#ifndef CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP

// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"

// Clang includes
#include <clang/AST/ASTConsumer.h>

// Standard includes
#include <string>

namespace clang {
class ASTContext;
class SourceLocation;
}

namespace ClangExpand {
struct Query;
}

namespace ClangExpand { namespace SymbolSearch {

/// \ingroup SymbolSearch
///
/// The `SymbolSearch::Consumer` is responsible for creating an appropriate
/// `ASTMatchers` expression and dispatching an AST visitor using clang's
/// ASTMatcher's library. For any match, our `SymbolSearch::MatchHandler` will
/// be invoked.
///
/// The `ASTMatchers` expression we create is quite complex, as it has to match
/// functions, methods, constructors and all derivatives thereof (though we get
/// those for free). The full (lisp-commented) matcher looks like this:
///
/// ```
/// expr(anyOf(                   ;; any expression
///  callExpr(anyOf(              ;; that is a call expression
///    hasDescendant(declRefExpr( ;; that is a function reference
///                               ;; with the name we want
///      hasDeclaration(functionDecl(hasName(spelling)).bind("fn")))
///    .bind("ref")),
///    hasDescendant(memberExpr(  ;; or a method reference
///                               ;; with the name we want
///      hasDeclaration(cxxMethodDecl(hasName(spelling)).bind("fn")))
///    .bind("member"))))
///  .bind("call"),
///  cxxConstructExpr(  ;; or is not a call expression, but a construction
///     hasDeclaration(
///       cxxConstructorDecl(
///         hasName(spelling), ;; with the name we want
///         isUserProvided())  ;; and is not compiler-generated
///       .bind("fn")))
///  .bind("construct")));
/// ```
class Consumer : public clang::ASTConsumer {
 public:
  /// Constructor.
  Consumer(const clang::SourceLocation& invocationLocation,
           std::string callSpelling,
           Query& query);

  /// Creates an appropriate match expression and dispatches the
  /// `SymbolSearch::MatchHandler`
  void HandleTranslationUnit(clang::ASTContext& context) override;

 private:
  /// The spelling (string representation) of the invoked function.
  const std::string _callSpelling;

  /// Our callback class for ASTMatcher matches.
  MatchHandler _matchHandler;
};

}  // namespace SymbolSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
