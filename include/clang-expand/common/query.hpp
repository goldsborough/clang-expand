#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/options.hpp"

#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"

// Standard includes
#include <optional>

namespace ClangExpand {

/// Stores the options and state of an ongoing query.
///
/// A `Query` object is created inside `run()` and passed through all stages of
/// the tool to collect and store data. After the search has finished, the
/// `Query` can be converted to a `Result` and finally printed to the console.
struct Query {
  /// Constructs a fresh `Query` with the given `Options`.
  explicit Query(Options options_) : options(options_) {
  }

  /// Utility method to test if it is necessary to collect `DeclarationData`.
  /// This will only be the case if the user requested `DeclarationData`, or if
  /// the user requested information about the definition of the function.
  bool requiresDeclaration() const noexcept {
    return options.wantsDeclaration || requiresDefinition();
  }

  /// Utility method to test if it is necessary to collect `DefinitionData`.
  /// This is the case if the user wants the original or rewritten definition,
  /// or both.
  bool requiresDefinition() const noexcept {
    return options.wantsDefinition || options.wantsRewritten;
  }

  /// Utility method to check, after symbol search, whether the search was not
  /// successful. This function respects command line options, i.e. if the query
  /// has no `DeclarationData` and the user did not request such data,
  /// `foundNothing` will not return true (report a failure).
  bool foundNothing() const noexcept {
    return requiresDeclaration() && (!declaration && !definition);
  }

  /// Possibly collected `CallData`.
  std::optional<CallData> call;

  /// Possibly collected `DeclarationData`.
  std::optional<DeclarationData> declaration;

  /// Possibly collected `DefinitionData`.
  std::optional<DefinitionData> definition;

  /// The `Options` of the query (i.e. what information the user wants).
  const Options options;
};

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
