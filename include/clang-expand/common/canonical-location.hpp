#ifndef CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP
#define CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace clang {
class FileEntry;
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {
struct CanonicalLocation {
  CanonicalLocation(const clang::SourceLocation& location,
                    const clang::SourceManager& sourceManager);

  bool operator==(const CanonicalLocation& other) const noexcept;
  bool operator!=(const CanonicalLocation& other) const noexcept;

  const clang::FileEntry* file;
  unsigned offset;
};
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_CANONICAL_LOCATION_HPP
