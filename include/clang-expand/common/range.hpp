#ifndef CLANG_EXPAND_COMMON_RANGE_HPP
#define CLANG_EXPAND_COMMON_RANGE_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/YAMLTraits.h>

// Standard includes
#include <string>

namespace clang {
class SourceManager;
class SourceRange;
}

namespace ClangExpand {
/// A range of source code, represented by a start and end offset.
struct Range {
  /// Constructs a range from a `clang::SourceRange` and `clang::SourceManager`, used to obtain the
  /// strat and end `Offset`s.
  Range(const clang::SourceRange& range, const clang::SourceManager& sourceManager);

  /// Constructor.
  Range(Offset begin_, Offset end_);

  /// The starting offset.
  Offset begin;

  /// The ending offset. May be inclusive or exclusive depending on the context.
  Offset end;
};
}  // namespace ClangExpand

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::Range> {
  static void mapping(llvm::yaml::IO& io, ClangExpand::Range& range) {
    io.mapRequired("begin", range.begin);
    io.mapRequired("end", range.end);
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_COMMON_RANGE_HPP
