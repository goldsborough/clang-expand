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
/// An offset into a file, represented by a `(line, column)` pair. Both the line and the column
/// start are 1-indexed, as they are in clang.
struct Offset {
  /// Constructs an `Offset` by converting a `clang::SourceLocation`.
  Offset(const clang::SourceLocation& location, const clang::SourceManager& sourceManager);

  /// Constructor.
  Offset(unsigned line_, unsigned column_);

  /// The 1-indexed line (row) of the location.
  unsigned line;

  /// The 1-indexed column (offset into the line) of the loction.
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
