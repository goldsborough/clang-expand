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
  using Callback = std::function<void(Query&&)>;

  Query();

  explicit Query(DeclarationData&& declaration);
  explicit Query(DefinitionData&& definition);
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

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_QUERY_HPP
