// Project includes
#include "clang-expand/common/range.hpp"

// Third party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/Basic/SourceManager.h>

namespace ClangExpand {
Range::Range(const clang::SourceRange& range,
             const clang::SourceManager& sourceManager)
: begin(range.getBegin(), sourceManager), end(range.getEnd(), sourceManager) {
}

Range::Range(Offset begin_, Offset end_) : begin(begin_), end(end_) {
}

nlohmann::json Range::toJson() const {
  // clang-format off
  return {
    {"begin", begin.toJson()},
    {"end", end.toJson()}
  };
  // clang-format on
}

}  // namespace ClangExpand
