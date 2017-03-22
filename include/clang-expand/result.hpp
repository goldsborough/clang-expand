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

#ifndef CLANG_EXPAND_RESULT_HPP
#define CLANG_EXPAND_RESULT_HPP

// Project includes
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/range.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>

// Third party includes
#include <third-party/json.hpp>

namespace llvm {
class raw_ostream;
}

namespace ClangExpand {
struct Query;
/// Stores the result of a `Query`.
///
/// Converting this structure to YAML gives the full (nested) output of
/// clang-expand, including the call range, declaration and definition
/// information.
struct Result {
  /// Constructs a `Result` from a completed `Query`.
  ///
  /// If the query's options specify that the call range is requested, the query
  /// must contain `CallData`.
  explicit Result(Query&& query);

  /// Converts the `Result` to JSON.
  nlohmann::json toJson() const;

  /// The range of the entire function call.
  ///
  /// This is the range that has to be replaced when expanding the tall.
  llvm::Optional<Range> callRange;

  /// The declaration data of the call.
  llvm::Optional<DeclarationData> declaration;

  /// The definition data of the call.
  llvm::Optional<DefinitionData> definition;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_RESULT_HPP
