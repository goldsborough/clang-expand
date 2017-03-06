#ifndef CLANG_EXPAND_COMMON_QUERY_HPP
#define CLANG_EXPAND_COMMON_QUERY_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Standard includes
#include <functional>
#include <optional>
#include <variant>

namespace ClangExpand {
class Query {
 public:
  Query();

  Query(DeclarationData&& declaration);  // NOLINT(runtime/explicit)
  Query(DefinitionData&& definition);    // NOLINT(runtime/explicit)
  Query(DeclarationData&& declaration, CallData&& call);

  bool isDefinition() const noexcept;
  bool isDeclaration() const noexcept;
  bool hasCall() const noexcept;

  const DeclarationData& declaration() const noexcept;
  const DefinitionData& definition() const noexcept;
  const CallData& call() const noexcept;

 private:
  using PartialState =
      std::variant<std::monostate, DeclarationData, DefinitionData>;
  using OptionalCall = std::optional<CallData>;

  PartialState _state;
  OptionalCall _call;
};

using QueryCallback = std::function<void(Query&&)>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
