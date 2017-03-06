// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/SourceManager.h>

// Standard includes
#include <utility>

namespace ClangExpand::Structures {
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
: filename(sourceManager.getFilename(location))
, line(sourceManager.getSpellingLineNumber(location))
, column(sourceManager.getSpellingColumnNumber(location)) {
}

EasyLocation::EasyLocation(const llvm::StringRef& filename_,
                           unsigned line_,
                           unsigned column_)
: filename(filename_), line(line_), column(column_) {
}
}  // namespace ClangExpand::Structures
