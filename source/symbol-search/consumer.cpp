// Project includes
#include "clang-expand/symbol-search/consumer.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>

// Standard includes
#include <string>

namespace ClangExpand::SymbolSearch {
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

Consumer::Consumer(const clang::SourceLocation& invocationLocation,
                   const std::string& invocationSpelling,
                   const LazyBoolean& alreadyFoundMacro,
                   Query* query)
: _callSpelling(invocationSpelling)
, _alreadyFoundMacro(alreadyFoundMacro)
, _matchHandler(invocationLocation, query) {
}

void Consumer::HandleTranslationUnit(clang::ASTContext& context) {
  // Safe to call at this point.
  if (_alreadyFoundMacro()) return;

  const auto matcher = createAstMatcher(_callSpelling);
  clang::ast_matchers::MatchFinder matchFinder;
  matchFinder.addMatcher(matcher, &_matchHandler);
  matchFinder.matchAST(context);
}

}  // namespace ClangExpand::SymbolSearch
