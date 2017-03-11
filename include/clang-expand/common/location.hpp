#ifndef CLANG_EXPAND_COMMON_STRUCTURES_HPP
#define CLANG_EXPAND_COMMON_STRUCTURES_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {
struct Location {
  Location(const clang::SourceLocation& location,
           const clang::SourceManager& sourceManager);

  Location(const llvm::StringRef& filename, unsigned line, unsigned column);

  std::string filename;
  Offset offset;
};
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_STRUCTURES_HPP
