#ifndef CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP

// Project includes
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>

// Standard includes
#include <optional>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace ClangExpand {
class Query;
}

namespace ClangExpand::DefinitionSearch {
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  explicit MatchHandler(Query& query);

  void run(const MatchResult& result) override;

 private:
  bool _matchParameters(const clang::ASTContext& context,
                        const clang::FunctionDecl& function) const noexcept;

  bool _matchContexts(const clang::FunctionDecl& function) const noexcept;

  Query& _query;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
