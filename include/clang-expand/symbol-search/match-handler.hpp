#ifndef CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace clang {
class SourceLocation;
}

namespace ClangExpand {
class Query;
}

namespace ClangExpand::SymbolSearch {
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  explicit MatchHandler(const clang::SourceLocation& targetLocation,
                        Query* query);

  void run(const MatchResult& result) override;

 private:
  const clang::SourceLocation& _targetLocation;
  Query* _query;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
