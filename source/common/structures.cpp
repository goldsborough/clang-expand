// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

namespace ClangExpand::Structures {
CanonicalLocation::CanonicalLocation(
    const clang::SourceLocation& location,
    const clang::SourceManager& sourceManager) {
  const auto decomposed = sourceManager.getDecomposedLoc(location);
  file = sourceManager.getFileEntryForID(decomposed.first);
  offset = decomposed.second;
}

bool CanonicalLocation::operator==(const CanonicalLocation& other) noexcept {
  return this->file == other.file && this->offset == other.offset;
}

bool CanonicalLocation::operator!=(const CanonicalLocation& other) noexcept {
  return !(*this == other);
}
}  // namespace ClangExpand::Structures
