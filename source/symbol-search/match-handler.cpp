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

auto collectState(const clang::FunctionDecl& function) {
  ClangExpand::DeclarationState state(function.getName());

  state.numberOfArguments = function.getNumParams();

  auto* context = function.getPrimaryContext()->getParent();
  for ( ; context; context = context->getParent()) {
    // state.context.push_back(context->get);
  }


  // const auto* c  = function.getEnclosingNamespaceContext();

  llvm::outs() << function.getPrimaryContext()->getParent()->getDeclKindName()
               << '\n';

  return state;
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

  _resultCallback(collectState(*function));
}

}  // namespace ClangExpand::SymbolSearch
