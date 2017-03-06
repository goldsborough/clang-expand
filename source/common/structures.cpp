// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/Basic/SourceManager.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <utility>

namespace ClangExpand {
CanonicalLocation::CanonicalLocation(
    const clang::SourceLocation& location,
    const clang::SourceManager& sourceManager) {
  const auto decomposed = sourceManager.getDecomposedLoc(location);
  file = sourceManager.getFileEntryForID(decomposed.first);
  offset = decomposed.second;
}

bool CanonicalLocation::operator==(const CanonicalLocation& other) const
    noexcept {
  return this->file == other.file && this->offset == other.offset;
}

bool CanonicalLocation::operator!=(const CanonicalLocation& other) const
    noexcept {
  return !(*this == other);
}

EasyLocation::EasyLocation(const clang::SourceLocation& location,
                           const clang::SourceManager& sourceManager)
: EasyLocation(sourceManager.getFilename(location),
               sourceManager.getSpellingLineNumber(location),
               sourceManager.getSpellingColumnNumber(location)) {
}

EasyLocation::EasyLocation(const llvm::StringRef& filename_,
                           unsigned line,
                           unsigned column)
: filename(filename_), offset{line, column} {
}
}  // namespace ClangExpand
