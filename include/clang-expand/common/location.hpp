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

#ifndef CLANG_EXPAND_COMMON_LOCATION_HPP
#define CLANG_EXPAND_COMMON_LOCATION_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// Third party includes
#include <third-party/json.hpp>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {

/// An easier-to-use representaiton of a source location.
///
/// `clang::SourceLocation`s are meant to be stored as efficiently as possible
/// in the AST to keep it small. `SourceLocation` are really just IDs or indices
/// into a "location-table", so this table must be consulted through the source
/// manager to go from a `SourceLocation` to the filename, line and/or column
/// that the `SourceLocation` represents. Meanwhile, this `Location` class is
/// much less space efficient but stores all important information inside (like
/// a "fat" `SourceLocation`). This is useful since we need such `Locations` a
/// lot when doing our processing as well as for final output to stdout.
struct Location {
  /// Constructs a `Location` from a `clang::SourceLocation` using the source
  /// manager.
  Location(const clang::SourceLocation& location,
           const clang::SourceManager& sourceManager);

  /// Constructs a `Location` from a filename and `(line, column)` pair.
  Location(const llvm::StringRef& filename_, unsigned line, unsigned column);

  /// Converts the `Location` to JSON.
  nlohmann::json toJson() const;

  /// The name of the file this location is from.
  std::string filename;

  /// The offset into the file (a `(line, column)` pair).
  Offset offset;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_LOCATION_HPP
