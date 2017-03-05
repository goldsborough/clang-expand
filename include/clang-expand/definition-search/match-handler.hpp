#ifndef CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/Basic/SourceLocation.h>

// Standard includes
#include <functional>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace ClangExpand::DefinitionSearch {
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  MatchHandler(const DeclarationState& declaration,
               const StateCallback& stateCallback);

  void run(const MatchResult& result) override;

 private:
  const DeclarationState& _declaration;
  const StateCallback _stateCallback;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
