// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/function-properties.hpp"

// Clang includes
#include "clang/AST/Decl.h"

// LLVM includes
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <cassert>

namespace ClangExpand::SymbolSearch {
MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           const ResultCallback& resultCallback)
: _targetLocation(targetLocation), _resultCallback(resultCallback) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr);

  // function->getNameInfo().getLoc().dump(*result.SourceManager);
  // function->dump();
  llvm::outs() << function << "\n";

  FunctionProperties properties;

  // if (callExpression) {
  //   llvm::outs() << "handler ok\n";
  // } else {
  //   llvm::outs() << "handler no\n";
  // }
}

}  // namespace ClangExpand::SymbolSearch
