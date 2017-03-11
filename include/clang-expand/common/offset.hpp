#ifndef CLANG_EXPAND_COMMON_OFFSET_HPP
#define CLANG_EXPAND_COMMON_OFFSET_HPP

// Standard includes
#include <string>

// LLVM includes
#include <llvm/Support/YAMLTraits.h>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {
struct Offset {
  Offset(const clang::SourceLocation& location,
         const clang::SourceManager& sourceManager);

  Offset(unsigned line_, unsigned column_);

  unsigned line;
  unsigned column;
};
}  // namespace ClangExpand

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::Offset> {
  static void mapping(llvm::yaml::IO& io, ClangExpand::Offset& offset) {
    io.mapRequired("line", offset.line);
    io.mapRequired("column", offset.column);
  }
};
}  // namespace llvm::yaml


#endif  // CLANG_EXPAND_COMMON_OFFSET_HPP
