#ifndef CLANG_EXPAND_COMMON_LOCATION_HPP
#define CLANG_EXPAND_COMMON_LOCATION_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/YAMLTraits.h>

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

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::Location> {
  static void mapping(llvm::yaml::IO& io, ClangExpand::Location& location) {
    io.mapRequired("file", location.filename);
    io.mapRequired("line", location.offset.line);
    io.mapRequired("column", location.offset.column);
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_COMMON_LOCATION_HPP
