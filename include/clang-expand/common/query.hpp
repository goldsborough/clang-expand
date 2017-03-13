#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/options.hpp"

#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"

// Standard includes
#include <functional>
#include <optional>

namespace ClangExpand {
struct Query {
  explicit Query(Options options_) : options(options_) {
  }

  bool requiresDeclaration() const noexcept {
    return options.wantsDeclaration || requiresDefinition();
  }

  bool requiresDefinition() const noexcept {
    return options.wantsDefinition || options.wantsRewritten;
  }

  bool foundNothing() const noexcept {
    return requiresDeclaration() && (!declaration && !definition);
  }

  std::optional<DeclarationData> declaration;
  std::optional<DefinitionData> definition;
  std::optional<CallData> call;
  const Options options;
};

using QueryCallback = std::function<void(Query&&)>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
