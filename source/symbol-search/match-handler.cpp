// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/AST/Decl.h"

// LLVM includes
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <cassert>

namespace ClangExpand::SymbolSearch {
namespace {
auto fileEntryAndOffset(const clang::SourceLocation& location,
                        const clang::SourceManager& sourceManager) {
  const auto decomposed = sourceManager.getDecomposedLoc(location);
  const auto* fileEntry = sourceManager.getFileEntryForID(decomposed.first);
  return std::make_pair(fileEntry, decomposed.second);
}

bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return fileEntryAndOffset(first, sourceManager) ==
         fileEntryAndOffset(second, sourceManager);
}

auto collectDeclarationState(const clang::FunctionDecl& function,
                             const clang::ASTContext& astContext) {
  ClangExpand::DeclarationState declaration(function.getName());

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
      declaration.contexts.emplace_back(kind, ns->getName());
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      declaration.contexts.emplace_back(kind, record->getName());
    }
  }

  return declaration;
}

auto collectDefinitionState(const clang::FunctionDecl& function) {
  ClangExpand::DefinitionState definition;

  return definition;
}

}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           const ResultCallback& resultCallback)
: _targetLocation(targetLocation), _resultCallback(resultCallback) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* call = result.Nodes.getNodeAs<clang::DeclRefExpr>("call");
  assert(call != nullptr);

  const auto& sourceManager = *result.SourceManager;
  const auto callLocation = call->getLocation();
  if (!locationsAreEqual(callLocation, _targetLocation, sourceManager)) {
    return;
  }

  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr);

  if (function->hasBody()) {
    // _resultCallback(collectDefinitionState(*function));
    _resultCallback(collectDeclarationState(*function, *result.Context));
  } else {
    _resultCallback(collectDeclarationState(*function, *result.Context));
  }
}

}  // namespace ClangExpand::SymbolSearch
