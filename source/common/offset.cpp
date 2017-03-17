// Project includes
#include "clang-expand/common/offset.hpp"

// Third party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/Basic/SourceManager.h>

namespace ClangExpand {
Offset::Offset(const clang::SourceLocation& location,
               const clang::SourceManager& sourceManager)
: line(sourceManager.getSpellingLineNumber(location))
, column(sourceManager.getSpellingColumnNumber(location)) {
}

Offset::Offset(unsigned line_, unsigned column_)
: line(line_), column(column_) {
}

nlohmann::json Offset::toJson() const {
  // clang-format off
  return {
    {"line", line},
    {"column", column}
  };
  // clang-format on
}

}  // namespace ClangExpand
