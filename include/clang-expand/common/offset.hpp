#ifndef CLANG_EXPAND_COMMON_OFFSET_HPP
#define CLANG_EXPAND_COMMON_OFFSET_HPP

// Standard includes
#include <string>

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


#endif  // CLANG_EXPAND_COMMON_OFFSET_HPP
