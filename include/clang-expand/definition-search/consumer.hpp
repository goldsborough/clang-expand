#ifndef CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP

// Library includes
#include "clang-expand/common/query.hpp"
#include "clang-expand/definition-search/match-handler.hpp"

// Clang includes
#include <clang/AST/ASTConsumer.h>

namespace clang {
class ASTContext;
}

namespace ClangExpand::DefinitionSearch {

class Consumer : public clang::ASTConsumer {
 public:
  Consumer(const DeclarationData& declaration,
           const QueryCallback& stateCallback);

  void HandleTranslationUnit(clang::ASTContext& context) override;

 private:
  const DeclarationData& _declaration;
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
