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

// Project includes
#include "clang-expand/symbol-search/consumer.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>

// Standard includes
#include <string>

namespace ClangExpand {
namespace SymbolSearch {
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

}  // namespace SymbolSearch
}  // namespace ClangExpand
