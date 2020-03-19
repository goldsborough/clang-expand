//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/range.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/options.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/DeclCXX.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/StmtIterator.h>
#include <clang/AST/Type.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Lex/Lexer.h>

// LLVM includes
#include <clang/AST/PrettyPrinter.h>
#include <llvm/ADT/None.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <iosfwd>
#include <iterator>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>

namespace ClangExpand {
namespace SymbolSearch {
namespace {
using ParameterMap = DeclarationData::ParameterMap;

/// Performs some necessary preprocessing on call ranges before we can plug them
/// into the `CallData` object returned from the match handler.
Range cleanCallRange(const clang::Expr& expression,
                     const clang::SourceRange& range,
                     const clang::ASTContext& context) {
  // For a normal call expression with parentheses, we have to add +1
  // because the call expression does not include the final semicolon.
  unsigned extraOffset = +1;
  if (const auto* op =
          llvm::dyn_cast<clang::CXXOperatorCallExpr>(&expression)) {
    // For an operator expression, the end of the call range is the first
    // character of the right operand (for binary operators) or only operand
    // (for unary operators), for some reason. So we have to skip the whole
    // token and are then already at the semicolon (so no +1).
    extraOffset = clang::Lexer::MeasureTokenLength(op->getEndLoc(),
                                                   context.getSourceManager(),
                                                   context.getLangOpts());
  }

  const auto begin = range.getBegin();
  const auto end = range.getEnd().getLocWithOffset(+extraOffset);

  return {{begin, end}, context.getSourceManager()};
}

/// Collects a `DeclarationData` object containing the declaration's location,
/// context and text.
auto collectDeclarationData(const clang::FunctionDecl& function,
                            clang::ASTContext& astContext,
                            ParameterMap&& parameterMap) {
  const Location location(function.getLocation(),
                          astContext.getSourceManager());
  ClangExpand::DeclarationData declaration(function.getNameAsString(),
                                           location);

  declaration.parameterMap = std::move(parameterMap);
  auto text = Routines::getSourceText(function.getSourceRange(), astContext);
  declaration.text = (std::move(text) + llvm::Twine(";")).str();

  const auto& policy = astContext.getPrintingPolicy();
  // Collect parameter types (their string representations)
  for (const auto* parameter : function.parameters()) {
    const auto type = parameter->getOriginalType().getCanonicalType();
    declaration.parameterTypes.emplace_back(type.getAsString(policy));
  }

  // Collect contexts (their kind, e.g. namespace or class, and name)
  const auto* context = function.getPrimaryContext()->getParent();
  for (; context; context = context->getParent()) {
    const auto kind = context->getDeclKind();
    if (auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      declaration.contexts.emplace_back(kind, ns->getName().str());
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      declaration.contexts.emplace_back(kind, record->getName().str());
    }
  }

  return declaration;
}

/// Inserts an entry into a parameter map, given the parameter declaration and
/// the Expr object of the matching function call argument.
void addParameterMapping(ParameterMap& parameters,
                         const clang::ParmVarDecl& parameter,
                         const clang::Expr& argument,
                         clang::ASTContext& context) {
  const auto range = argument.getSourceRange();
  const auto callName = Routines::getSourceText(range, context);
  const auto originalName = parameter.getName();

  parameters.insert({originalName, callName});
}

/// Tests the two required properties for a call expression to be a member
/// operator overload call: (1) the call is an operator call expression and (2)
/// the operator is a method.
bool isMemberOperatorOverloadCall(const clang::CallExpr& call) {
  return llvm::isa<clang::CXXOperatorCallExpr>(call) &&
         llvm::isa<clang::CXXMethodDecl>(call.getDirectCallee());
}

/// Required overload for `mapCallParameters()` to compile. See the `if
/// constexpr` in `mapCallParameters` for how this could be avoided in C++17.
template <
    typename T,
    typename = std::enable_if_t<!std::is_base_of<clang::CallExpr, T>::value>>
bool isMemberOperatorOverloadCall(const T&) {
  return false;
}

/// Attempts to perform the parameter mapping for member operator overloads,
/// which are particularly tricky as they have fewer call arguments than
/// function parameters. Returns a null optinal if the call is not an operator
/// overload, else the correct parameter map for the unary of binary operator
/// overload.
ParameterMap
mapParametersForOperatorOverloads(const clang::CallExpr& call,
                                  const clang::FunctionDecl& function,
                                  clang::ASTContext& context) {
  // If this is a binary member operator overload, the second argument is
  // the 'other' parameter of the function declaration (i.e. #params = 1,
  // #args = 2!).

  ParameterMap parameters;
  if (llvm::cast<clang::CXXOperatorCallExpr>(call).isInfixBinaryOp()) {
    addParameterMapping(parameters,
                        **function.param_begin(),
                        **std::next(call.arg_begin()),
                        context);
  }

  return parameters;
}

/// Necessary overload for `mapCallParameters()` to compile. Not acually used,
/// but cannot SFINAE away because no `if constexpr`.
template <typename... Ts>
auto mapParametersForOperatorOverloads(Ts&&...) {
  return ParameterMap();
}

/// Returns a `ParameterMap`, mapping function parameter names (the variables in
/// the declaration) to function call arguments (the expressions passed). The
/// call can have either CallExpr or CXXConstructExpr type.
template <typename CallOrConstruction>
ParameterMap mapCallParameters(const CallOrConstruction& call,
                               const clang::FunctionDecl& function,
                               clang::ASTContext& context) {
  // if constexpr(std::is_base_of_v<clang::CallExpr, CallOrConstruction>) {
  if (isMemberOperatorOverloadCall(call)) {
    return mapParametersForOperatorOverloads(call, function, context);
  }
  // }

  ParameterMap parameters;
  auto parameter = function.param_begin();
  for (const auto* argument : call.arguments()) {
    argument = argument->IgnoreImplicit();

    const auto* defaultArgument =
        llvm::dyn_cast<clang::CXXDefaultArgExpr>(argument);
    if (defaultArgument) argument = defaultArgument->getExpr();

    assert(parameter != function.param_end() &&
           "Function has more parameters than arguments?");
    addParameterMapping(parameters, **parameter, *argument, context);

    ++parameter;
  }

  return parameters;
}

/// Necessary overload for `parentAs()` to compile. See `if constexpr` inside
/// `parentAs()` for how this can be avoided in C++17.
template <
    typename Node,
    typename = std::enable_if_t<!std::is_base_of<clang::Expr, Node>::value>>
bool isImplicitExpression(const Node&, const clang::Stmt&) {
  return false;
}

/// Tests if the parent of a node is an implicit expression that should be
/// ignored.
bool isImplicitExpression(const clang::Expr& child, const clang::Expr& parent) {
  // If we ignore all implicit types on the way from the parent to the child
  // node and we are back at the child node, then the parent must have been an
  // implicit type.
  return parent.IgnoreImplicit() == &child;
}

/// Attempts to retrieve the parent of a node as the given type. It tries to
/// ignore implicit nodes that may hide th actual parent, e.g.
/// ImplicitCastExprs.
template <typename T, typename Node>
const T* parentAs(const Node& node, clang::ASTContext& context) {
  // Only the TranslationUnitDecl would have no parents, and we
  // should never deal with a TranslationUnitDecl directly.
  const auto parents = context.getParents(node);
  assert(!parents.empty() && "Orphan node?");

  // First check if the parent is the wanted type.
  const auto parent = parents.begin();
  if (const auto* wantedType = parent->template get<T>()) {
    return wantedType;
  }

  // Else, this may be an implicit expression like ExprWithCleanups or an
  // ImplicitCastExpr. If that is the case, we recurse below and look one level
  // up. If not, then the parent is some other kind and simply is not of type T.
  // Note that the parent can only be an implicit expression if the node it
  // wraps (i.e. the child) is an Expr.
  // if constexpr(std::is_base_of_v<clang::Expr, Node>) {
  if (const auto* parentStatement = parent->template get<clang::Expr>()) {
    if (isImplicitExpression(node, *parentStatement)) {
      return parentAs<T>(*parentStatement, context);
    }
  }
  // }

  // Parent is not the right type.
  return nullptr;
}

/// Finds out if a variable declaration is nested inside some statement where
/// we
/// don't want to expand the initializing function call. This it the case for
/// if
/// clauses (`if (int x = f())`) or for loop initializers, for example. Note
/// that this function actually attempts to determine the opposite, i.e. it
/// returns false if the variable is global or in a compound statement and
/// true
/// in all other cases.
bool isNestedInsideSomeOtherStatement(const clang::VarDecl& variable,
                                      clang::ASTContext& context) {
  // Make sure the parents are [DeclStmt[->CompoundStmt]]
  // or TranslationUnitDecl.
  if (parentAs<clang::TranslationUnitDecl>(variable, context)) return false;

  if (auto parent = parentAs<clang::DeclStmt>(variable, context)) {
    if (auto grandparent = parentAs<clang::CompoundStmt>(*parent, context)) {
      (void)grandparent;
      return false;
    }
  }

  return true;
}

/// Returns a string representation of a qualified type suitable for printing
/// (not for serialization and comparison!).
std::string getTypeAsString(const clang::QualType& qualType,
                            clang::ASTContext& context) {
  auto policy = context.getPrintingPolicy();
  policy.SuppressTagKeyword = true;
  policy.SuppressUnwrittenScope = true;
  policy.AnonymousTagLocations = false;
  policy.IncludeNewlines = false;
  return qualType.getAsString(policy);
}

/// For the case that the surrounding context of the function call is a
/// variable
/// declaration (e.g. in `int x = f(5);`), this function handles such a call.
/// It
/// makes sure this declaration is not in some bad location, e.g. inside an
/// `if`
/// clause. It also figures out if the assigned variable's type is
/// default-constructible, which is important in the case that the function
/// being called has at least one return statement that is not on the top
/// level
/// of the function (in which case an assignment for an expansion would be
/// invalid).
llvm::Optional<CallData> handleCallForVarDecl(const clang::VarDecl& variable,
                                              clang::ASTContext& context,
                                              const clang::Expr& expression) {
  // Could be an IfStmt, a WhileStmt, a CallExpr etc. etc.
  if (isNestedInsideSomeOtherStatement(variable, context)) {
    return llvm::None;
  }

  const auto qualType = variable.getType().getCanonicalType();
  const auto typeString = getTypeAsString(qualType, context);
  auto assignee = AssigneeData::Builder()
                      .type(typeString)
                      .name(variable.getName())
                      .op("=")
                      .build();

  const auto* type = qualType.getTypePtr();
  if (qualType.isConstQualified() || type->isReferenceType()) {
    assignee.type->isDefaultConstructible = false;
  } else if (const auto* record = type->getAsCXXRecordDecl()) {
    if (!record->hasDefaultConstructor()) {
      assignee.type->isDefaultConstructible = false;
    }
  }

  auto range = cleanCallRange(expression, variable.getSourceRange(), context);
  return CallData(std::move(assignee), std::move(range));
}

/// If we determined that the surrounding context of the function call has a
/// binary operator (like an assignment or compound operation, e.g. +=), then
/// this function takes care of handling that call and collecting relevant
/// data.
CallData
handleCallForBinaryOperator(const clang::BinaryOperator& binaryOperator,
                            clang::ASTContext& context,
                            const clang::Expr& expression) {
  const auto* lhs = binaryOperator.getLHS();
  if (&expression == lhs) {
    Routines::error(
        "Refuse to expand function that is LHS of a binary operator");
  }

  if (!binaryOperator.isAssignmentOp() &&
      !binaryOperator.isCompoundAssignmentOp() &&
      !binaryOperator.isShiftAssignOp()) {
    Routines::error("Cannot expand call as operand of " +
                    llvm::Twine(binaryOperator.getOpcodeStr()));
  }

  std::string name;
  if (const auto* declRefExpr = llvm::dyn_cast<clang::DeclRefExpr>(lhs)) {
    name = declRefExpr->getDecl()->getName().str();
  } else {
    // This may be a member expression, a function call or something else. But
    // since it's not a declaration, we can be quite safe to plop this into
    // each
    // return statement.
    name = Routines::getSourceText(lhs->getSourceRange(), context);
  }

  auto assignee = AssigneeData::Builder()
                      .name(name)
                      .op(binaryOperator.getOpcodeStr())
                      .build();

  auto range =
      cleanCallRange(expression, binaryOperator.getSourceRange(), context);
  return {std::move(assignee), std::move(range)};
}

/// Attempts to obtain `CallData` from the surroundings (context) of an
/// expression by walking up the AST a certain number of levels until it finds
/// something we can handle (like a return statement or a variable
/// declaration).
/// If the maximum recursion ("walking-up") depth is reached, the operation
/// fails. The depth value passed must initially not be zero.
llvm::Optional<CallData>
collectCallDataFromContext(const clang::Expr& expression,
                           clang::ASTContext& context,
                           unsigned depth = 8) {
  // Not checking the base case is generally bad for the first call, but we
  // don't actually want this to be called with depth = 0 the first time.
  assert(depth > 0 && "Reached invalid depth while walking up call expression");

  for (const auto parent : context.getParents(expression)) {
    if (const auto* node = parent.get<clang::ReturnStmt>()) {
      return CallData(
          cleanCallRange(expression, node->getSourceRange(), context));
    } else if (const auto* node = parent.get<clang::VarDecl>()) {
      return handleCallForVarDecl(*node, context, expression);
    } else if (const auto* node = parent.get<clang::BinaryOperator>()) {
      return handleCallForBinaryOperator(*node, context, expression);
    }
  }

  // You could call this a BFS that favors the first parents, or simply a
  // mixture of BFS and DFS, since we first walk all parents, but then recurse
  // into the first parent (so it's neither DFS not BFS, but something that
  // should work better for our purposes).
  if (depth > 1) {
    for (const auto parent : context.getParents(expression)) {
      if (const auto* node = parent.get<clang::Expr>()) {
        auto result = collectCallDataFromContext(*node, context, depth - 1);
        if (result) return result;
      }
    }
  }

  // Found no call :(
  return llvm::None;
}

/// Obtains the most accurate location of the function/method/constructor
/// invocation depending on what exactly we matched.
clang::SourceLocation getCallLocation(const MatchHandler::MatchResult& result) {
  if (auto* ref = result.Nodes.getNodeAs<clang::DeclRefExpr>("ref")) {
    return ref->getLocation();
  }

  if (const auto* memberCall =
          result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("call")) {
    if (memberCall->getMethodDecl()->isOverloadedOperator()) {
      // Since we only lex one token in the action (we have very primitive
      // tools
      // down there), non-infix operator calls have to be recognized by the
      // location of the operator token (e.g. '<<' or '~' or '=') and not the
      // actual function, which begins at the 'operator' token.
      return memberCall->getExprLoc().getLocWithOffset(+8);
    }
  }

  if (auto* member = result.Nodes.getNodeAs<clang::MemberExpr>("member")) {
    return member->getMemberLoc();
  }

  const auto* constructor =
      result.Nodes.getNodeAs<clang::CXXConstructExpr>("construct");
  assert(constructor && "Found no callable in match result");

  return constructor->getLocation();
}

/// Returns a pointer into the raw character-level source buffer at the given
/// location, using the result's source manager. This is an alternative way of
/// getting at the raw source text next to Routines::getSourceText. It doesn't
/// always work, but happens to work here, and should be more efficient.
const char* bufferPointerAt(const clang::SourceLocation& location,
                            const MatchHandler::MatchResult& result) {
  bool error;
  const char* data = result.SourceManager->getCharacterData(location, &error);
  assert(!error && "Error getting character data pointer");

  return data;
}

/// Collects information w.r.t. any member whose method is begin called. For
/// example, if the function expanded is `x.f()`, then we'll want to store the
/// *base* `x` so that we can prefix all member expressions inside the function
/// with this name.
void decorateCallDataWithMemberBase(CallData& callData,
                                    const MatchHandler::MatchResult& result) {
  if (auto* call = result.Nodes.getNodeAs<clang::CallExpr>("call")) {
    if (isMemberOperatorOverloadCall(*call)) {
      const auto lhs = *(call->arg_begin());
      callData.base =
          Routines::getSourceText(lhs->getSourceRange(), *result.Context);
      callData.base += ".";
      return;
    }
  }

  if (auto* member = result.Nodes.getNodeAs<clang::MemberExpr>("member")) {
    const auto it = member->child_begin();
    const clang::Expr* child = (const clang::Expr*)*it;
    child = child->IgnoreImplicit();
    if (!llvm::isa<clang::CXXThisExpr>(child)) {
      const char* start = bufferPointerAt(member->getBeginLoc(), result);
      const char* end = bufferPointerAt(member->getMemberLoc(), result);
      callData.base.assign(start, end);
      return;
    }
  }

  const auto* constructor =
      result.Nodes.getNodeAs<clang::CXXConstructorDecl>("fn");
  if (constructor && callData.assignee.hasValue()) {
    callData.base = callData.assignee->name + ".";
  }
}

/// Collects the call expression and the parameter map for a function call.
std::pair<const clang::Expr*, ParameterMap>
inspectCall(const clang::FunctionDecl& function,
            const MatchHandler::MatchResult& result) {
  auto& context = *result.Context;
  if (auto* functionCall = result.Nodes.getNodeAs<clang::CallExpr>("call")) {
    auto parameterMap = mapCallParameters(*functionCall, function, context);
    return {functionCall, std::move(parameterMap)};
  }
  const auto* constructor =
      result.Nodes.getNodeAs<clang::CXXConstructExpr>("construct");
  auto parameterMap = mapCallParameters(*constructor, function, context);

  return {constructor, std::move(parameterMap)};
}

/// Obtains information about the function call circumstances. This includes the
/// range of the entire function call (including any variables that are assigned
/// the return value of the function), any base (object whose method is called,
/// when the function is a method) as well as data about any assignee.
CallData collectCallData(const clang::Expr& call, clang::ASTContext& context) {
  // If the parent is a compound statement or a translation unit (for globals),
  // this is a plain function call (i.e. simply `^f(x);$`), so only need the
  // range.
  if (parentAs<clang::CompoundStmt>(call, context) ||
      parentAs<clang::TranslationUnitDecl>(call, context)) {
    return CallData(cleanCallRange(call, call.getSourceRange(), context));
  }

  if (auto optional = collectCallDataFromContext(call, context)) {
    return *optional;
  }

  // We only match for what we know are OK expressions, because the set of bad
  // expressions is much greater. For example, we don't want to expand function
  // calls inside other function calls, inside `if` conditions, inside for loop
  // declarations or any other locations where we're not safely expanding into a
  // compound statment that allows more than one statment instead of the
  // original expression.
  Routines::error("Refuse or unable to expand at given location");
}

/// Checks if the call location obtained through the match result matches the
/// target location.
bool callLocationMatches(const MatchHandler::MatchResult& result,
                         const clang::SourceLocation& targetLocation) {
  const auto& sourceManager = *result.SourceManager;
  const auto callLocation = getCallLocation(result);
  return Routines::locationsAreEqual(callLocation,
                                     targetLocation,
                                     sourceManager);
}
}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           Query& query)
: _targetLocation(targetLocation), _query(query) {
}

void MatchHandler::run(const MatchResult& result) {
  if (!callLocationMatches(result, _targetLocation)) return;

  // This is either a pure FunctionDecl, a CXXMethodDecl or a CXXConstructorDecl
  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function && "Did not match required function declaration");

  const clang::Expr* callExpression;
  ParameterMap parameterMap;

  std::tie(callExpression, parameterMap) = inspectCall(*function, result);

  assert(callExpression &&
         "Matched neither function call nor constructor invocation");

  auto& context = *result.Context;

  if (_query.options.wantsCall || _query.options.wantsRewritten) {
    auto callData = collectCallData(*callExpression, context);
    decorateCallDataWithMemberBase(callData, result);
    _query.call = std::move(callData);
  }

  // Already found a macro definition
  if (_query.definition) return;

  if (_query.requiresDeclaration()) {
    _query.declaration =
        collectDeclarationData(*function, context, std::move(parameterMap));
  }

  if (_query.requiresDefinition() && function->hasBody()) {
    _query.definition = DefinitionData::Collect(*function, context, _query);
  }
}

}  // namespace SymbolSearch
}  // namespace ClangExpand
