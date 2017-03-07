#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/data.hpp"

// Standard includes
#include <functional>
#include <optional>
#include <variant>

namespace ClangExpand {
class Query {
 public:
  using OptionalCall = std::optional<CallData>;

  Query();

  Query(DeclarationData&& declaration);  // NOLINT(runtime/explicit)
  Query(DefinitionData&& definition);    // NOLINT(runtime/explicit)
  Query(DeclarationData&& declaration, OptionalCall&& call);

  bool isDefinition() const noexcept;
  bool isDeclaration() const noexcept;
  bool hasCall() const noexcept;
  bool isEmpty() const noexcept;

  explicit operator bool() const noexcept;

  const DeclarationData& declaration() const noexcept;
  const DefinitionData& definition() const noexcept;
  const OptionalCall& call() const noexcept;

 private:
  using PartialState =
      std::variant<std::monostate, DeclarationData, DefinitionData>;

  PartialState _state;
  OptionalCall _call;
};

using QueryCallback = std::function<void(Query&&)>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
