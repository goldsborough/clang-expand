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
/// Creates an ASTMatcher matching function or method call expressions as well
/// as constructor invocations.
auto createAstMatcher(const std::string& spelling) {
  using namespace clang::ast_matchers;  // NOLINT(build/namespaces)
  // clang-format off
  return expr(anyOf(
           callExpr(anyOf(
             hasDescendant(declRefExpr(
               hasDeclaration(functionDecl(hasName(spelling)).bind("fn")))
             .bind("ref")),
             hasDescendant(memberExpr(
               hasDeclaration(cxxMethodDecl(hasName(spelling)).bind("fn")))
             .bind("member"))))
           .bind("call"),
           cxxConstructExpr(
              hasDeclaration(
                cxxConstructorDecl(
                  hasName(spelling),
                  isUserProvided())
                .bind("fn")))
           .bind("construct")));
  // clang-format on
}
}  // namespace

Consumer::Consumer(const clang::SourceLocation& invocationLocation,
                   std::string callSpelling,
                   Query& query)
: _callSpelling(std::move(callSpelling))
, _matchHandler(invocationLocation, query) {
}

void Consumer::HandleTranslationUnit(clang::ASTContext& context) {
  const auto matcher = createAstMatcher(_callSpelling);
  clang::ast_matchers::MatchFinder matchFinder;
  matchFinder.addMatcher(matcher, &_matchHandler);
  matchFinder.matchAST(context);
}

}  // namespace ClangExpand::SymbolSearch
