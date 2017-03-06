// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
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
                             const clang::ASTContext& astContext) {
  ClangExpand::DeclarationData declaration(function.getName());

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
}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           const StateCallback& stateCallback)
: _targetLocation(targetLocation), _stateCallback(stateCallback) {
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

  if (function->hasBody()) {
    auto definition =
        Routines::collectDefinitionData(*function, context, parameterMap);
    _stateCallback(std::move(definition));
  } else {
    auto declaration = collectDeclarationData(*function, context);
    declaration.parameterMap = std::move(parameterMap);
    _stateCallback(std::move(declaration));
  }
}

}  // namespace ClangExpand::SymbolSearch
