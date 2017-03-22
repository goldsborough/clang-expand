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

#ifndef CLANG_EXPAND_COMMON_CALL_DATA_HPP
#define CLANG_EXPAND_COMMON_CALL_DATA_HPP

// Project includes
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/range.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>

// Standard includes
#include <string>

namespace ClangExpand {

/// Stores information about a function call.
struct CallData {
  /// Constructs the `CallData` with the extent of the call, leaving the `base`
  /// string empty and the `assignee` data null.
  explicit CallData(Range&& extent_);

  /// Constructs the `CallData` with an `assignee` and the extent of the call.
  CallData(AssigneeData&& assignee, Range&& extent_);

  /// Utility function to check if declaring the type of the assignee of the
  /// function return value is required. This is the case if there is an
  /// assignee at all and we picked up the type of the assignee (which would not
  /// be the case for *assignments* as opposed to constructions).
  bool requiresDeclaration() const noexcept;

  /// Any base expression to the function call, e.g. the `object` in
  /// `object.method()`.
  std::string base;

  /// Information about the assignee of the call expression, including things
  /// like the type and name of the variable as well as the assignment operator
  /// (we also refer to compound operators like `+=` to assignments in this
  /// case).
  llvm::Optional<AssigneeData> assignee;

  /// The source range of the entire call expression, from the first character
  /// of any variable declaration to the final semicolon. The semicolon is
  /// included in the range.
  Range extent;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_CALL_DATA_HPP
