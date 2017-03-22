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

#ifndef CLANG_EXPAND_COMMON_OFFSET_HPP
#define CLANG_EXPAND_COMMON_OFFSET_HPP

// Third party includes
#include <third-party/json.hpp>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {
/// An offset into a file, represented by a `(line, column)` pair. Both the line
/// and the column start are 1-indexed, as they are in clang.
struct Offset {
  /// Constructs an `Offset` by converting a `clang::SourceLocation`.
  Offset(const clang::SourceLocation& location,
         const clang::SourceManager& sourceManager);

  /// Constructor.
  Offset(unsigned line_, unsigned column_);

  /// Converts the `Offset` to JSON.
  nlohmann::json toJson() const;

  /// The 1-indexed line (row) of the location.
  unsigned line;

  /// The 1-indexed column (offset into the line) of the loction.
  unsigned column;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_OFFSET_HPP
