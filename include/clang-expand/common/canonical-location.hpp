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

#ifndef CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP
#define CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP

namespace clang {
class FileEntry;
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {

/// A location structure that always compares equal for identical offset.
///
/// `clang::SourceLocation`s don't always compare equal, even if they point at
/// the exact same location. What does always, work, however, is checking that
/// the files of the two `clang::SourceLocation`s and the offsets into those
/// files are the same
struct CanonicalLocation {
  /// Converts a `clang::SourceLocation` into a `CanonicalLocation`.
  CanonicalLocation(const clang::SourceLocation& location,
                    const clang::SourceManager& sourceManager);

  /// Tests two `CanonicalLocation`s for equality.
  bool operator==(const CanonicalLocation& other) const noexcept;

  /// Tests two `CanonicalLocation`s for inequality.
  bool operator!=(const CanonicalLocation& other) const noexcept;

  /// The file entry of the location.
  const clang::FileEntry* file;

  /// The offset (index) into the file.
  unsigned offset;
};
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP
