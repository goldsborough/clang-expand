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
/// This `clang::ASTConsumer` is a very simple class that receives the ongoing
/// `Query` instance with information collected during symbol search and matches
/// on all functions with the same *name* (only) as the one whose declaration we
/// found. The only other thing we can match for is that the function have a
/// definiton, since that is what we are interested in in this phase.
class Consumer : public clang::ASTConsumer {
 public:
  /// Constructor, taking the ongoing `Query` object.
  explicit Consumer(Query& query);

  /// Creates an ASTMatcher expression and dispatches it on the translation
  /// unit. The goal is to find functions with the same names as the function
  /// found in the `DeclarationData`.
  void HandleTranslationUnit(clang::ASTContext& context) override;

 private:
  /// The ongoing `Query` object.
  Query& _query;

  /// The match handler the consumer will dispatch.
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
