#ifndef CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/SourceLocation.h"

// Standard includes
#include <functional>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace ClangExpand::SymbolSearch {
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  explicit MatchHandler(const clang::SourceLocation& targetLocation,
                        const StateCallback& stateCallback);

  void run(const MatchResult& result) override;

 private:
  const clang::SourceLocation _targetLocation;
  const StateCallback _stateCallback;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
