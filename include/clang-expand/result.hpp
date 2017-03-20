#ifndef CLANG_EXPAND_RESULT_HPP
#define CLANG_EXPAND_RESULT_HPP

// Project includes
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/range.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>

// Third party includes
#include <third-party/json.hpp>

namespace llvm {
class raw_ostream;
}

namespace ClangExpand {
struct Query;
/// Stores the result of a `Query`.
///
/// Converting this structure to YAML gives the full (nested) output of
/// clang-expand, including the call range, declaration and definition
/// information.
struct Result {
  /// Constructs a `Result` from a completed `Query`.
  ///
  /// If the query's options specify that the call range is requested, the query
  /// must contain `CallData`.
  explicit Result(Query&& query);

  /// Converts the `Result` to JSON.
  nlohmann::json toJson() const;

  /// The range of the entire function call.
  ///
  /// This is the range that has to be replaced when expanding the tall.
  llvm::Optional<Range> callRange;

  /// The declaration data of the call.
  llvm::Optional<DeclarationData> declaration;

  /// The definition data of the call.
  llvm::Optional<DefinitionData> definition;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_RESULT_HPP
