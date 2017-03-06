#ifndef CLANG_EXPAND_COMMON_STRUCTURES_HPP
#define CLANG_EXPAND_COMMON_STRUCTURES_HPP

// LLVM includes
#include <llvm/ADT/StringRef.h>

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

struct Offset {
  unsigned line;
  unsigned column;
};

struct Range {
  Offset begin;
  Offset end;
};

struct EasyLocation {
  EasyLocation(const clang::SourceLocation& location,
               const clang::SourceManager& sourceManager);

  EasyLocation(const llvm::StringRef& filename, unsigned line, unsigned column);

  llvm::StringRef filename;
  Offset offset;
};
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_STRUCTURES_HPP
