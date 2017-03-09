// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/ASTTypeTraits.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/Type.h>
#include <clang/ASTMatchers/ASTMatchers.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <cstdlib>
#include <iterator>
#include <string>
#include <type_traits>

namespace ClangExpand::SymbolSearch {
namespace {
auto collectDeclarationData(const clang::FunctionDecl& function,
                            const clang::ASTContext& astContext,
                            ParameterMap&& parameterMap) {
  ClangExpand::DeclarationData declaration(function.getName());
  declaration.parameterMap = std::move(parameterMap);

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
                            const clang::ASTContext& context) {
  const auto& sourceManager = context.getSourceManager();
  const auto& languageOptions = context.getLangOpts();
  const auto originalName = parameter.getName();
  const auto range = argument.getSourceRange();
  const auto callName = Routines::getSourceText(range,
                                                sourceManager,
                                                languageOptions,
                                                /*offsetAtEnd=*/+1);
  parameters.insert({originalName, callName});
}

bool isMemberOperatorOverload(const clang::CallExpr& call,
                              const clang::FunctionDecl& function) {
  if (!llvm::isa<clang::CXXOperatorCallExpr>(call)) return false;
  if (!llvm::isa<clang::CXXMethodDecl>(function)) return false;
  return true;
}

ParameterMap mapCallParameters(const clang::CallExpr& call,
                               const clang::FunctionDecl& function,
                               const clang::ASTContext& context) {
  ParameterMap parameters;

  // If this is a member operator overload, the second argument is the 'other'
  // parameter of the function declaration (i.e. #params = 1, #args = 2!).
  if (isMemberOperatorOverload(call, function)) {
    insertParameterMapping(parameters,
                           **function.param_begin(),
                           **std::next(call.arg_begin()),
                           context);
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

CallData handleCallForVarDecl(const clang::VarDecl& variable,
                              const clang::ASTContext& context) {
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
  return {std::move(assignee), std::move(range)};
}

llvm::StringRef entireMemberExpressionString(const clang::MemberExpr& member,
                                             const clang::ASTContext& context) {
  // There are so many different kinds of member expressions like x.x, x.X::x,
  // x->x, x-> template x etc. that it's easiest to just grab the source.
  // FIXME: if this becomes a performance issue.
  return Routines::getSourceText(member.getSourceRange(),
                                 context.getSourceManager(),
                                 context.getLangOpts(),
                                 /*offsetAtEnd=*/+2);
}

CallData
handleCallForBinaryOperator(const clang::BinaryOperator& binaryOperator,
                            const clang::ASTContext& context) {
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
    name = entireMemberExpressionString(*member, context);
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
    } else if (const auto* node = parent.get<clang::CallExpr>()) {
      Routines::error("Cannot expand call inside another call expression");
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
  if (const auto* ref = result.Nodes.getNodeAs<clang::DeclRefExpr>("ref")) {
    return ref->getLocation();
  }

  const auto* member = result.Nodes.getNodeAs<clang::MemberExpr>("member");
  assert(member && "Found neither a function nor a method in match");

  return member->getMemberLoc();
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

void decorateCallDataWithMemberBase(std::optional<CallData>& callData,
                                    const MatchHandler::MatchResult& result) {
  assert(callData.has_value() && "Should have call data at this point");

  if (auto* member = result.Nodes.getNodeAs<clang::MemberExpr>("member")) {
    const auto* child = member->child_begin()->IgnoreImplicit();
    if (!llvm::isa<clang::CXXThisExpr>(child)) {
      const char* start = bufferPointerAt(member->getLocStart(), result);
      const char* end = bufferPointerAt(member->getMemberLoc(), result);
      callData->base.assign(start, end);
    }
  }
}

}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           Query* query)
: _targetLocation(targetLocation), _query(query) {
}

void MatchHandler::run(const MatchResult& result) {
  if (!callLocationMatches(result, _targetLocation)) return;

  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr);

  auto& context = *result.Context;
  const auto* call = result.Nodes.getNodeAs<clang::CallExpr>("call");
  auto parameterMap = mapCallParameters(*call, *function, context);

  std::optional<CallData> callData = collectCallDataFromContext(*call, context);
  if (!callData.has_value()) {
    // If we found no "context" (i.e. assignment or return statement) to collect
    // information about and the range of the full statement, this is probably
    // just a plain function call like f() or foo.bar(). In that case we simply
    // pick the call expression as the range.
    callData.emplace(Range(call->getSourceRange(), context.getSourceManager()));
  }

  decorateCallDataWithMemberBase(callData, result);

  if (function->hasBody()) {
    *_query = Routines::collectDefinitionData(*function,
                                              context,
                                              parameterMap,
                                              callData);
  } else {
    auto declaration =
        collectDeclarationData(*function, context, std::move(parameterMap));
    *_query = Query(std::move(declaration), std::move(callData));
  }
}

}  // namespace ClangExpand::SymbolSearch
