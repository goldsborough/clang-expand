#ifndef CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP

// Library includes
#include "clang-expand/definition-search/match-handler.hpp"

// Clang includes
#include <clang/AST/ASTConsumer.h>

namespace clang {
class ASTContext;
class MacroInfo;
class FunctionDecl;
class Token;
}

namespace ClangExpand {
struct DeclarationState;
}

namespace ClangExpand::DefinitionSearch {

class Consumer : public clang::ASTConsumer {
 public:
  Consumer(const DeclarationState& declaration,
           const StateCallback& stateCallback);

  void HandleTranslationUnit(clang::ASTContext& Context) override;

 private:
  const DeclarationState& _declaration;
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
