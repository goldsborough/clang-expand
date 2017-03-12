#ifndef CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP

// Project includes
#include "clang-expand/definition-search/match-handler.hpp"

// Clang includes
#include <clang/AST/ASTConsumer.h>

namespace clang {
class ASTContext;
}

namespace ClangExpand {
struct Query;
}

namespace ClangExpand::DefinitionSearch {

class Consumer : public clang::ASTConsumer {
 public:
  explicit Consumer(Query& query);

  void HandleTranslationUnit(clang::ASTContext& context) override;

 private:
  Query& _query;
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
