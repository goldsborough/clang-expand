#ifndef CLANG_EXPAND_COMMON_OFFSET_HPP
#define CLANG_EXPAND_COMMON_OFFSET_HPP

// Third party includes
#include <third-party/json.hpp>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {
/// An offset into a file, represented by a `(line, column)` pair. Both the line
/// and the column start are 1-indexed, as they are in clang.
struct Offset {
  /// Constructs an `Offset` by converting a `clang::SourceLocation`.
  Offset(const clang::SourceLocation& location,
         const clang::SourceManager& sourceManager);

  /// Constructor.
  Offset(unsigned line_, unsigned column_);

  /// Converts the `Offset` to JSON.
  nlohmann::json toJson() const;

  /// The 1-indexed line (row) of the location.
  unsigned line;

  /// The 1-indexed column (offset into the line) of the loction.
  unsigned column;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_OFFSET_HPP
