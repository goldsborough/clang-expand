// Project includes
#include "clang-expand/common/offset.hpp"

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
}  // namespace ClangExpand
