#ifndef CLANG_EXPAND_RESULT_HPP
#define CLANG_EXPAND_RESULT_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/Support/YAMLTraits.h>

// Standard includes
#include <iosfwd>
#include <optional>

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

  /// The range of the entire function call.
  ///
  /// This is the range that has to be replaced when expanding the tall.
  std::optional<Range> callRange;

  /// The declaration data of the call.
  std::optional<DeclarationData> declaration;

  /// The definition data of the call.
  std::optional<DefinitionData> definition;
};

llvm::raw_ostream& operator<<(llvm::raw_ostream& stream, const Result& result);
}  // namespace ClangExpand

namespace llvm::yaml {
/// Serialization traits for YAML output.
template <>
struct MappingTraits<ClangExpand::Result> {
  static void mapping(llvm::yaml::IO& io, ClangExpand::Result& result) {
    if (result.callRange) {
      io.mapRequired("call", *result.callRange);
    }
    if (result.declaration) {
      io.mapRequired("declaration", *result.declaration);
    }
    if (result.definition) {
      io.mapRequired("definition", *result.definition);
    }
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_RESULT_HPP
