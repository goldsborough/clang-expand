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

#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/options.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>

namespace ClangExpand {

/// Stores the options and state of an ongoing query.
///
/// A `Query` object is created inside `run()` and passed through all stages of
/// the tool to collect and store data. After the search has finished, the
/// `Query` can be converted to a `Result` and finally printed to the console.
struct Query {
  /// Constructs a fresh `Query` with the given `Options`.
  explicit Query(Options options_) : options(options_) {
  }

  /// Utility method to test if it is necessary to collect `DeclarationData`.
  /// This will only be the case if the user requested `DeclarationData`, or if
  /// the user requested information about the definition of the function.
  bool requiresDeclaration() const noexcept {
    return options.wantsDeclaration || requiresDefinition();
  }

  /// Utility method to test if it is necessary to collect `DefinitionData`.
  /// This is the case if the user wants the original or rewritten definition,
  /// or both.
  bool requiresDefinition() const noexcept {
    return options.wantsDefinition || options.wantsRewritten;
  }

  /// Utility method to check, after symbol search, whether the search was not
  /// successful. This function respects command line options, i.e. if the query
  /// has no `DeclarationData` and the user did not request such data,
  /// `foundNothing` will not return true (report a failure).
  bool foundNothing() const noexcept {
    return requiresDeclaration() && (!declaration && !definition);
  }

  /// Possibly collected `CallData`.
  llvm::Optional<CallData> call;

  /// Possibly collected `DeclarationData`.
  llvm::Optional<DeclarationData> declaration;

  /// Possibly collected `DefinitionData`.
  llvm::Optional<DefinitionData> definition;

  /// The `Options` of the query (i.e. what information the user wants).
  const Options options;
};

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
