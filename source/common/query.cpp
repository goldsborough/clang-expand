// Project includes
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/data.hpp"

// Standard includes
#include <optional>
#include <variant>

namespace ClangExpand {
Query::Query() = default;

Query::Query(DeclarationData&& declaration) : _state(std::move(declaration)) {
}

Query::Query(DefinitionData&& definition) : _state(std::move(definition)) {
}

Query::Query(DeclarationData&& declaration, OptionalCall&& call)
: _state(std::move(declaration)), _call(std::move(call)) {
}

bool Query::isDefinition() const noexcept {
  return std::holds_alternative<DefinitionData>(_state);
}

bool Query::isDeclaration() const noexcept {
  return std::holds_alternative<DeclarationData>(_state);
}

bool Query::hasCall() const noexcept {
  return _call.has_value();
}

bool Query::isEmpty() const noexcept {
  return _state.index() == 0;
}

Query::operator bool() const noexcept {
  return !isEmpty();
}

const DeclarationData& Query::declaration() const noexcept {
  assert(isDeclaration() &&
         "Query state does not currently hold a declaration");
  return std::get<DeclarationData>(_state);
}

const DefinitionData& Query::definition() const noexcept {
  assert(isDefinition() && "Query state does not currently hold a definition");
  return std::get<DefinitionData>(_state);
}
const Query::OptionalCall& Query::call() const noexcept {
  assert(hasCall() && "Query does not currently hold a call");
  return _call;
}

}  // namespace ClangExpand
