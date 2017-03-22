//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

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

namespace ClangExpand {
namespace SymbolSearch {

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
