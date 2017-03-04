#ifndef CLANG_EXPAND_COMMON_STRUCTURES_HPP
#define CLANG_EXPAND_COMMON_STRUCTURES_HPP

namespace clang {
class FileEntry;
class SourceLocation;
class SourceManager;
}

namespace ClangExpand::Structures {
struct CanonicalLocation {
  CanonicalLocation(const clang::SourceLocation& location,
                    const clang::SourceManager& sourceManager);

  bool operator==(const CanonicalLocation& other) const noexcept;
  bool operator!=(const CanonicalLocation& other) const noexcept;

  const clang::FileEntry* file;
  unsigned offset;
};
}  // namespace ClangExpand::Structures


#endif  // CLANG_EXPAND_COMMON_STRUCTURES_HPP
