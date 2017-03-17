#ifndef CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP

// Project includes
#include "clang-expand/common/location.hpp"

// Third party includes
#include <third-party/json.hpp>

// Standard includes
#include <string>

namespace clang {
class FunctionDecl;
class ASTContext;
}

namespace ClangExpand {
struct Query;

/// Stores data about the definition of a function.
struct DefinitionData {
  /// Collects the entire `DefinitionData` for the function. The `query` passed
  /// should already have been through symbol search and must store `CallData`
  /// and `DeclarationData`.
  static DefinitionData Collect(const clang::FunctionDecl& function,
                                clang::ASTContext& context,
                                const Query& query);

  /// Converts the `DefinitionData` to JSON.
  nlohmann::json toJson() const;

  /// The `Location` of the definition in the source.
  Location location;

  /// The original, untouched source text of the definition.
  std::string original;

  /// The rewritten (expanded) source text of the definition.
  std::string rewritten;

  /// Whether this definition is from a macro or a real function.
  bool isMacro{false};
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
