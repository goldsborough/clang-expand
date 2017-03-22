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

#ifndef CLANG_EXPAND_COMMON_RANGE_HPP
#define CLANG_EXPAND_COMMON_RANGE_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// Third party includes
#include <third-party/json.hpp>

namespace clang {
class SourceManager;
class SourceRange;
}

namespace ClangExpand {
/// A range of source code, represented by a start and end offset.
struct Range {
  /// Constructs a range from a `clang::SourceRange` and `clang::SourceManager`,
  /// used to obtain the
  /// strat and end `Offset`s.
  Range(const clang::SourceRange& range,
        const clang::SourceManager& sourceManager);

  /// Constructor.
  Range(Offset begin_, Offset end_);

  /// Converts the `Range` to JSON.
  nlohmann::json toJson() const;

  /// The starting offset.
  Offset begin;

  /// The ending offset. May be inclusive or exclusive depending on the context.
  Offset end;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_RANGE_HPP
