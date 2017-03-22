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

namespace ClangExpand {
namespace DefinitionSearch {

/// \ingroup DefinitionSearch
///
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

}  // namespace DefinitionSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_CONSUMER_HPP
