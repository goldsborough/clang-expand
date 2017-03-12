// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/range.hpp"
#include "clang-expand/common/routines.hpp"

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

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <iterator>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>

namespace ClangExpand::SymbolSearch {
namespace {
using ParameterMap = DeclarationData::ParameterMap;

auto collectDeclarationData(const clang::FunctionDecl& function,
                            clang::ASTContext& astContext,
                            ParameterMap&& parameterMap) {
  const Location location(function.getLocation(),
                          astContext.getSourceManager());
  ClangExpand::DeclarationData declaration(function.getName(), location);

  declaration.parameterMap = std::move(parameterMap);
  const auto text =
      Routines::getSourceText(function.getSourceRange(), astContext);
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

void insertParameterMapping(ParameterMap& parameters,
                            const clang::ParmVarDecl& parameter,
                            const clang::Expr& argument,
                            clang::ASTContext& context) {
  const auto range = argument.getSourceRange();
  const auto callName = Routines::getSourceText(range, context);
  const auto originalName = parameter.getName();

  parameters.insert({originalName, callName});
}

bool isMemberOperatorOverloadCall(const clang::CallExpr& call) {
  return llvm::isa<clang::CXXOperatorCallExpr>(call) &&
         llvm::isa<clang::CXXMethodDecl>(call.getDirectCallee());
}

bool isMemberOperatorOverloadCall(const clang::CXXConstructExpr&) {
  return false;
}

template <typename CallOrConstruction>
ParameterMap mapCallParameters(const CallOrConstruction& call,
                               const clang::FunctionDecl& function,
                               clang::ASTContext& context) {
  ParameterMap parameters;


  if (isMemberOperatorOverloadCall(call)) {
    if (llvm::cast<clang::CXXOperatorCallExpr>(call).isInfixBinaryOp()) {
      // If this is a binary member operator overload, the second argument is
      // the 'other' parameter of the function declaration (i.e. #params = 1,
      // #args = 2!).
      insertParameterMapping(parameters,
                             **function.param_begin(),
                             **std::next(call.arg_begin()),
                             context);
    }
    return parameters;
  }

  auto parameter = function.param_begin();
  for (const auto* argument : call.arguments()) {
    argument = argument->IgnoreImplicit();

    // We only want to map argument that were actually passed in the call
    if (llvm::isa<clang::CXXDefaultArgExpr>(argument)) continue;

    assert(parameter != function.param_end() &&
           "Function has more parameters than arguments?");
    insertParameterMapping(parameters, **parameter, *argument, context);

    ++parameter;
  }

  return parameters;
}

template <typename T, typename Node>
std::optional<const T*>
parentIfHasType(const Node& node, clang::ASTContext& context) {
  const auto parents = context.getParents(node);
  if (parents.empty()) return std::nullopt;
  return parents.begin()->template get<T>();
}

bool isNestedInsideSomeOtherStatment(const clang::VarDecl& variable,
                                     clang::ASTContext& context) {
  // Make sure the parents are [DeclStmt[->CompoundStmt]].
  if (auto parent = parentIfHasType<clang::DeclStmt>(variable, context)) {
    // Found the parent and it's not what we wanted it to be.
    if (parent.value() == nullptr) return true;
    if (auto grandparent =
            parentIfHasType<clang::CompoundStmt>(**parent, context)) {
      if (grandparent.value() == nullptr) return true;
    }
  }

  return false;
}

std::optional<CallData> handleCallForVarDecl(const clang::VarDecl& variable,
                                             clang::ASTContext& context) {
  // Could be an IfStmt, a WhileStmt, a CallExpr etc. etc.
  if (isNestedInsideSomeOtherStatment(variable, context)) {
    return std::nullopt;
  }

  const auto qualType = variable.getType().getCanonicalType();
  const auto* type = qualType.getTypePtr();
  const auto& policy = context.getPrintingPolicy();
  auto assignee = AssigneeData::Builder()
                      .type(qualType.getAsString(policy))
                      .name(variable.getName())
                      .op("=")
                      .build();

  if (qualType.isConstQualified() || type->isReferenceType()) {
    assignee.type->isDefaultConstructible = false;
  } else if (const auto* record = type->getAsCXXRecordDecl(); record) {
    if (!record->hasDefaultConstructor()) {
      assignee.type->isDefaultConstructible = false;
    }
  }

  Range range(variable.getSourceRange(), context.getSourceManager());
  return CallData(std::move(assignee), std::move(range));
}

CallData
handleCallForBinaryOperator(const clang::BinaryOperator& binaryOperator,
                            clang::ASTContext& context) {
  if (!binaryOperator.isAssignmentOp() &&
      !binaryOperator.isCompoundAssignmentOp() &&
      !binaryOperator.isShiftAssignOp()) {
    Routines::error("Cannot expand call as operand of " +
                    llvm::Twine(binaryOperator.getOpcodeStr()));
  }

  std::string name;

  const auto* lhs = binaryOperator.getLHS();
  if (const auto* declRefExpr = llvm::dyn_cast<clang::DeclRefExpr>(lhs)) {
    name = declRefExpr->getDecl()->getName();
  } else if (const auto* member = llvm::dyn_cast<clang::MemberExpr>(lhs)) {
    // There are so many different kinds of member expressions like x.x, x.X::x,
    // x->x, x-> template x etc. that it's easiest to just grab the source.
    // FIXME: if this becomes a performance issue.
    name = Routines::getSourceText(member->getSourceRange(), context);
  } else {
    Routines::error("Cannot expand call because assignee is not recognized");
  }

  auto assignee = AssigneeData::Builder()
                      .name(name)
                      .op(binaryOperator.getOpcodeStr())
                      .build();

  Range range(binaryOperator.getSourceRange(), context.getSourceManager());

  return {std::move(assignee), std::move(range)};
}

std::optional<CallData>
collectCallDataFromContext(const clang::Expr& expression,
                           clang::ASTContext& context,
                           unsigned depth = 8) {
  // Not checking the base case is generally bad for the first call, but we
  // don't actually want this to be called with depth = 0 the first time.
  assert(depth > 0 && "Reached invalid depth while walking up call expression");

  for (const auto parent : context.getParents(expression)) {
    if (const auto* node = parent.get<clang::ReturnStmt>()) {
      return CallData({node->getSourceRange(), context.getSourceManager()});
    } else if (const auto* node = parent.get<clang::VarDecl>()) {
      return handleCallForVarDecl(*node, context);
    } else if (const auto* node = parent.get<clang::BinaryOperator>()) {
      return handleCallForBinaryOperator(*node, context);
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
  return {};
}

clang::SourceLocation getCallLocation(const MatchHandler::MatchResult& result) {
  if (auto* ref = result.Nodes.getNodeAs<clang::DeclRefExpr>("ref")) {
    return ref->getLocation();
  }

  if (const auto* memberCall =
          result.Nodes.getNodeAs<clang::CXXMemberCallExpr>("call")) {
    if (memberCall->getMethodDecl()->isOverloadedOperator()) {
      // Since we only lex one token in the action (we have very primitive tools
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

bool callLocationMatches(const MatchHandler::MatchResult& result,
                         const clang::SourceLocation& targetLocation) {
  const auto& sourceManager = *result.SourceManager;
  const auto callLocation = getCallLocation(result);
  return Routines::locationsAreEqual(callLocation,
                                     targetLocation,
                                     sourceManager);
}

const char* bufferPointerAt(const clang::SourceLocation& location,
                            const MatchHandler::MatchResult& result) {
  bool error;
  const char* data = result.SourceManager->getCharacterData(location, &error);
  assert(!error && "Error getting character data pointer");

  return data;
}

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
    const auto* child = member->child_begin()->IgnoreImplicit();
    if (!llvm::isa<clang::CXXThisExpr>(child)) {
      const char* start = bufferPointerAt(member->getLocStart(), result);
      const char* end = bufferPointerAt(member->getMemberLoc(), result);
      callData.base.assign(start, end);
      return;
    }
  }

  const auto* constructor =
      result.Nodes.getNodeAs<clang::CXXConstructorDecl>("fn");
  if (constructor && callData.assignee.has_value()) {
    callData.base = callData.assignee->name + ".";
  }
}

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

CallData collectCallData(const clang::Expr& call, clang::ASTContext& context) {
  if (auto parent = parentIfHasType<clang::CompoundStmt>(call, context)) {
    if (parent.value() != nullptr) {
      return CallData({call.getSourceRange(), context.getSourceManager()});
    }
  }

  if (auto optional = collectCallDataFromContext(call, context)) {
    return optional.value();
  }

  Routines::error("Refuse or unable to expand at given location");
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

  auto[call, parameterMap] = inspectCall(*function, result);

  assert(call && "Matched neither function call nor constructor invocation");

  auto& context = *result.Context;
  auto callData = collectCallData(*call, context);

  decorateCallDataWithMemberBase(callData, result);
  _query.call = std::move(callData);

  // Already found a macro definition
  if (_query.definition) return;

  auto declaration =
      collectDeclarationData(*function, context, std::move(parameterMap));
  _query.declaration = std::move(declaration);

  if (function->hasBody()) {
    auto definition = DefinitionData::Collect(*function, context, _query);
    _query.definition = std::move(definition);
  }
}

}  // namespace ClangExpand::SymbolSearch
