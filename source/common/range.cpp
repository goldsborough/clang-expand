// Project includes
#include "clang-expand/common/range.hpp"

// Clang includes
#include <clang/Basic/SourceManager.h>

namespace ClangExpand {
Range::Range(const clang::SourceRange& range,
             const clang::SourceManager& sourceManager)
: begin(range.getBegin(), sourceManager), end(range.getEnd(), sourceManager) {
}

Range::Range(Offset begin_, Offset end_) : begin(begin_), end(end_) {
}
}  // namespace ClangExpand
