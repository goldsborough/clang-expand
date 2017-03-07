// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"

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

ParameterMap mapCallParameters(const clang::CallExpr& call,
                               const clang::FunctionDecl& function,
                               const clang::ASTContext& context) {
  ParameterMap expressions;
  const auto& sourceManager = context.getSourceManager();
  const auto& languageOptions = context.getLangOpts();

  auto parameter = function.param_begin();
  for (const auto* argument : call.arguments()) {
    // We only want to map argument that were actually passed in the call...
    if (llvm::isa<clang::CXXDefaultArgExpr>(argument)) continue;

    const auto originalName = (*parameter)->getName();
    const auto range = argument->getSourceRange();
    const auto callName =
        Routines::getSourceText(range, sourceManager, languageOptions);
    expressions.insert({originalName, callName});

    ++parameter;
  }

  return expressions;
}

CallData handleCallForAssignment(const clang::VarDecl& variable,
                                 const clang::ASTContext& context) {
  const auto& policy = context.getPrintingPolicy();
  const auto type = variable.getType().getCanonicalType().getAsString(policy);
  const auto name = variable.getName();
  Range range(variable.getSourceRange(), context.getSourceManager());

  return {type, name, range};
}

std::optional<CallData> collectCallData(const clang::Expr& expression,
                                        clang::ASTContext& context,
                                        unsigned depth = 3) {
  // Not checking the base case is generally bad for the first call, but we
  // don't actually want this to be called with depth = 0 the first time.
  assert(depth > 0 && "Reached invalid depth while walking up call expression");

  for (const auto parent : context.getParents(expression)) {
    const auto kind = parent.getNodeKind();
    llvm::outs() << kind.asStringRef() << '\n';
    if (const auto* node = parent.get<clang::ReturnStmt>()) {
      llvm::outs() << "Return\n";
      return CallData({node->getSourceRange(), context.getSourceManager()});
    } else if (const auto* node = parent.get<clang::CallExpr>()) {
      llvm::outs() << "Call\n";
      return {};
    } else if (const auto* node = parent.get<clang::VarDecl>()) {
      llvm::outs() << "Ass\n";
      return handleCallForAssignment(*node, context);
    }
    llvm::outs() << "Matched none!" << '\n';
  }

  // You could call this a BFS that favors the first parents, or simply a
  // mixture of BFS and DFS, since we first walk all parents, but then recurse
  // into the first parent (so it's neither DFS not BFS, but something that
  // should work fine for us).
  if (depth > 1) {
    for (const auto parent : context.getParents(expression)) {
      if (const auto* node = parent.get<clang::Expr>()) {
        if (auto result = collectCallData(*node, context, depth - 1); result) {
          return result;
        }
      }
    }
  }

  return {};
}
}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           Query* query)
: _targetLocation(targetLocation), _query(query) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* ref = result.Nodes.getNodeAs<clang::DeclRefExpr>("ref");
  assert(ref != nullptr);

  const auto& sourceManager = *result.SourceManager;
  const auto callLocation = ref->getLocation();
  const auto foundRightCall =
      Routines::locationsAreEqual(callLocation, _targetLocation, sourceManager);
  if (!foundRightCall) return;

  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr);

  auto& context = *result.Context;
  const auto* call = result.Nodes.getNodeAs<clang::CallExpr>("call");
  auto parameterMap = mapCallParameters(*call, *function, context);

  auto callData = collectCallData(*call, context);
  llvm::outs() << "found call data: " << callData.has_value() << '\n';

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
