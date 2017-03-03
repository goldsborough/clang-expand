// Library includes
#include "clang-expand/symbol-search/consumer.hpp"

// Clang includes
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/Token.h"

// Standard includes
#include <string>

namespace ClangExpand::SymbolSearch {
namespace {
auto createAstMatcher(const std::string& spelling) {
  using namespace clang::ast_matchers;  // NOLINT(build/namespaces)
  // clang-format off
  return callExpr(hasDescendant(declRefExpr(
           hasDeclaration(functionDecl(hasName(spelling)).bind("fn")))
             .bind("ref")));
  // clang-format on
}
}  // namespace

Consumer::Consumer(const clang::SourceLocation& invocationLocation,
                   const std::string& invocationSpelling,
                   const LazyMacroGetter& macroGetter,
                   const ResultCallback& resultCallback)
: _callSpelling(invocationSpelling)
, _macroGetter(macroGetter)
, _matchHandler(invocationLocation, resultCallback) {
}

void Consumer::HandleTranslationUnit(clang::ASTContext& context) {
  // Safe to call at this point.
  if (_macroGetter() != nullptr) return;

  const auto matcher = createAstMatcher(_callSpelling);
  clang::ast_matchers::MatchFinder matchFinder;
  matchFinder.addMatcher(matcher, &_matchHandler);
  matchFinder.matchAST(context);
}

}  // namespace ClangExpand::SymbolSearch
