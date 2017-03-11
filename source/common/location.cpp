// Project includes
#include "clang-expand/common/location.hpp"

// Clang includes
#include <clang/Basic/SourceManager.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

namespace ClangExpand {
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
