// Library includes
#include "clang-expand/definition-search/match-handler.hpp"
#include "clang-expand/common/parameter-rewriter.hpp"

// Clang includes
#include <clang/AST/Decl.h>
#include <clang/AST/RecursiveASTVisitor.h>
#include <clang/Rewrite/Core/Rewriter.h>

// Standard includes
#include <cassert>
#include <string>

namespace ClangExpand::DefinitionSearch {
MatchHandler::MatchHandler(const DeclarationState& declaration,
                           const StateCallback& stateCallback)
: _declaration(declaration), _stateCallback(stateCallback) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr && "Got null function node in match handler");

  if (function->getNumParams() != _declaration.parameterTypes.size()) return;

  const auto& policy = result.Context->getPrintingPolicy();

  auto expectedType = _declaration.parameterTypes.begin();
  for (const auto* parameter : function->parameters()) {
    const auto type = parameter->getOriginalType().getCanonicalType();
    if (*expectedType != type.getAsString(policy)) return;
    ++expectedType;
  }

  auto expectedContext = _declaration.contexts.begin();
  const auto* context = function->getPrimaryContext()->getParent();
  for (; context; context = context->getParent()) {
    const auto kind = context->getDeclKind();
    if (kind != expectedContext->kind) return;
    if (auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      if (ns->getName() != expectedContext->name) return;
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      if (record->getName() != expectedContext->name) return;
    }
  }

  llvm::outs() << "Found correct definition at ";
  function->getLocation().dump(*result.SourceManager);
  llvm::outs() << '\n';
}

}  // namespace ClangExpand::DefinitionSearch
