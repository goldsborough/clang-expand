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

Offset::Offset(const clang::SourceLocation& location,
               const clang::SourceManager& sourceManager)
: line(sourceManager.getSpellingLineNumber(location))
, column(sourceManager.getSpellingColumnNumber(location)) {
}

Offset::Offset(unsigned line_, unsigned column_)
: line(line_), column(column_) {
}

Range::Range(const clang::SourceRange& range,
             const clang::SourceManager& sourceManager)
: begin(range.getBegin(), sourceManager), end(range.getEnd(), sourceManager) {
}

Range::Range(Offset begin_, Offset end_) : begin(begin_), end(end_) {
}

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
}  // namespace ClangExpand
