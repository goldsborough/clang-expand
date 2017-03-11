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
struct Range {
  Range(const clang::SourceRange& range,
        const clang::SourceManager& sourceManager);

  Range(Offset begin_, Offset end_);

  Offset begin;
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
