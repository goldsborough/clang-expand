#ifndef CLANG_EXPAND_COMMON_RANGE_HPP
#define CLANG_EXPAND_COMMON_RANGE_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>

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


#endif  // CLANG_EXPAND_COMMON_RANGE_HPP
