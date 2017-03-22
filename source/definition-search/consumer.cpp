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
#include "clang-expand/definition-search/consumer.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>
#include <clang/ASTMatchers/ASTMatchers.h>
#include <clang/ASTMatchers/ASTMatchersInternal.h>

// LLVM includes
#include <llvm/ADT/Optional.h>

namespace ClangExpand {
namespace DefinitionSearch {
namespace {

/// Creates an ASTMatcher expression matching on functions that have a
/// definition and the same name as the function whose declaration we serialized
/// into the `DeclarationData` object.
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
}  // namespace DefinitionSearch
}  // namespace ClangExpand
