// Project includes
#include "clang-expand/definition-search/consumer.hpp"

#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>

namespace ClangExpand::DefinitionSearch {
namespace {
auto createAstMatcher(const DeclarationData& declaration) {
  using namespace clang::ast_matchers;  // NOLINT(build/namespaces)
  return functionDecl(isDefinition(), hasName(declaration.name)).bind("fn");
}
}  // namespace

Consumer::Consumer(Query& query) : _query(query), _matchHandler(query) {
}

void Consumer::HandleTranslationUnit(clang::ASTContext& context) {
  const auto matcher = createAstMatcher(*_query.declaration);
  clang::ast_matchers::MatchFinder matchFinder;
  matchFinder.addMatcher(matcher, &_matchHandler);
  matchFinder.matchAST(context);
}
}  // namespace ClangExpand::DefinitionSearch
