#ifndef CLANG_EXPAND_COMMON_LOCATION_HPP
#define CLANG_EXPAND_COMMON_LOCATION_HPP

// Project includes
#include "clang-expand/common/offset.hpp"

// Third party includes
#include <third-party/json.hpp>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand {

/// An easier-to-use representaiton of a source location.
///
/// `clang::SourceLocation`s are meant to be stored as efficiently as possible
/// in the AST to keep it small. `SourceLocation` are really just IDs or indices
/// into a "location-table", so this table must be consulted through the source
/// manager to go from a `SourceLocation` to the filename, line and/or column
/// that the `SourceLocation` represents. Meanwhile, this `Location` class is
/// much less space efficient but stores all important information inside (like
/// a "fat" `SourceLocation`). This is useful since we need such `Locations` a
/// lot when doing our processing as well as for final output to stdout.
struct Location {
  /// Constructs a `Location` from a `clang::SourceLocation` using the source
  /// manager.
  Location(const clang::SourceLocation& location,
           const clang::SourceManager& sourceManager);

  /// Constructs a `Location` from a filename and `(line, column)` pair.
  Location(const llvm::StringRef& filename_, unsigned line, unsigned column);

  /// Converts the `Location` to JSON.
  nlohmann::json toJson() const;

  /// The name of the file this location is from.
  std::string filename;

  /// The offset into the file (a `(line, column)` pair).
  Offset offset;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_LOCATION_HPP
