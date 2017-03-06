// Library includes
#include "clang-expand/definition-search/match-handler.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Type.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/Basic/SourceLocation.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <cassert>
#include <string>
#include <type_traits>

namespace ClangExpand::DefinitionSearch {
namespace {
template <typename Context>
bool contextMatches(const Context& context,
                    const ExpectedContext& expectedContext) {
  if (context.getDeclKind() != expectedContext.kind) return false;
  if (context.getName() != expectedContext.name) return false;
  return true;
}
}  // namespace

MatchHandler::MatchHandler(const DeclarationData& declaration,
                           const StateCallback& stateCallback)
: _declaration(declaration), _stateCallback(stateCallback) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr && "Got null function node in match handler");

  if (function->getNumParams() != _declaration.parameterTypes.size()) return;

  if (!_matchParameters(*result.Context, *function)) return;
  if (!_matchContexts(*function)) return;

  llvm::outs() << "Found correct definition at ";
  function->getLocation().dump(*result.SourceManager);
  llvm::outs() << '\n';

  auto state = Routines::collectDefinitionData(*function,
                                                *result.Context,
                                                _declaration.parameterMap);
  _stateCallback(std::move(state));
}

bool MatchHandler::_matchParameters(const clang::ASTContext& context,
                                    const clang::FunctionDecl& function) const
    noexcept {
  const auto& policy = context.getPrintingPolicy();

  auto expectedType = _declaration.parameterTypes.begin();
  for (const auto* parameter : function.parameters()) {
    const auto type = parameter->getOriginalType().getCanonicalType();
    if (*expectedType != type.getAsString(policy)) return false;
    ++expectedType;
  }

  return true;
}

bool MatchHandler::_matchContexts(const clang::FunctionDecl& function) const
    noexcept {
  auto expectedContext = _declaration.contexts.begin();

  const auto* context = function.getPrimaryContext()->getParent();
  for (; context; context = context->getParent()) {
    if (auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      if (!contextMatches(*ns, *expectedContext)) return false;
      ++expectedContext;
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      if (!contextMatches(*record, *expectedContext)) return false;
      ++expectedContext;
    }
  }

  return true;
}

}  // namespace ClangExpand::DefinitionSearch
