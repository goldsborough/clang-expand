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
#include "clang-expand/common/location.hpp"

// Third party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/Basic/SourceManager.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

namespace ClangExpand {
Location::Location(const clang::SourceLocation& location,
                   const clang::SourceManager& sourceManager)
: filename(sourceManager.getFilename(location))
, offset(location, sourceManager) {
}

Location::Location(const llvm::StringRef& filename_,
                   unsigned line,
                   unsigned column)
: filename(filename_), offset{line, column} {
}

nlohmann::json Location::toJson() const {
  // clang-format off
  return {
    {"filename", filename},
    {"offset", offset.toJson()}
  };
  // clang-format on
}

}  // namespace ClangExpand
