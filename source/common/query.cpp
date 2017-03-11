// Project includes
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/data.hpp"

// Standard includes
#include <optional>
#include <variant>

namespace ClangExpand {
Query::Query(bool shouldRewrite_) : shouldRewrite(shouldRewrite_) {
}

bool Query::hasDefinition() const noexcept {
  return definition.has_value();
}

bool Query::hasDeclaration() const noexcept {
  return declaration.has_value();
}

}  // namespace ClangExpand
