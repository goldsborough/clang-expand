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
