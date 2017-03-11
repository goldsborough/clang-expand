#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/data.hpp"

// Standard includes
#include <functional>
#include <optional>

namespace ClangExpand {
class Query {
 public:
  explicit Query(bool shouldRewrite_);

  bool hasDefinition() const noexcept;
  bool hasDeclaration() const noexcept;

  std::optional<DeclarationData> declaration;
  std::optional<DefinitionData> definition;
  std::optional<CallData> call;
  const bool shouldRewrite;
};

using QueryCallback = std::function<void(Query&&)>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
