// Project includes
#include "clang-expand/common/structures.hpp"
#include "clang-expand/common/routines.hpp"

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return Structures::CanonicalLocation(first, sourceManager) ==
         Structures::CanonicalLocation(second, sourceManager);
}
}  // namespace ClangExpand::Routines
