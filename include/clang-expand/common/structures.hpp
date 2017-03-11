#ifndef CLANG_EXPAND_COMMON_STRUCTURES_HPP
#define CLANG_EXPAND_COMMON_STRUCTURES_HPP

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace clang {
class FileEntry;
class SourceLocation;
class SourceManager;
class SourceRange;
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
  Offset(const clang::SourceLocation& location,
         const clang::SourceManager& sourceManager);

  Offset(unsigned line_, unsigned column_);

  unsigned line;
  unsigned column;
};

struct Range {
  Range(const clang::SourceRange& range,
        const clang::SourceManager& sourceManager);

  Range(Offset begin_, Offset end_);

  Offset begin;
  Offset end;
};

struct Location {
  Location(const clang::SourceLocation& location,
               const clang::SourceManager& sourceManager);

  Location(const llvm::StringRef& filename, unsigned line, unsigned column);

  std::string filename;
  Offset offset;
};
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_STRUCTURES_HPP
