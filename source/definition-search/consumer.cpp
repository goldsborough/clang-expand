// Library includes
#include "clang-expand/definition-search/consumer.hpp"

// Clang includes
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/Token.h"

// Standard includes
#include <string>

namespace ClangExpand::DefinitionSearch {
namespace {
auto createAstMatcher(const std::string& spelling) {
  using namespace clang::ast_matchers;  // NOLINT(build/namespaces)
  // clang-format off
  return callExpr(hasDescendant(
           declRefExpr(
             hasDeclaration(functionDecl(hasName(spelling)).bind("fn")))
           .bind("ref")))
         .bind("call");
  // clang-format on
}
}  // namespace

Consumer::Consumer(const DeclarationState& declaration,
                   const StateCallback& stateCallback)
: _declaration(declaration), _matchHandler(declaration, stateCallback) {
}

void Consumer::HandleTranslationUnit(clang::ASTContext& context) {
  const auto matcher = createAstMatcher("");
  clang::ast_matchers::MatchFinder matchFinder;
  matchFinder.addMatcher(matcher, &_matchHandler);
  matchFinder.matchAST(context);
}
}  // namespace ClangExpand::DefinitionSearch
