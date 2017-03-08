// Project includes
#include "clang-expand/common/call-data.hpp"

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {
CallData::CallData(AssigneeData&& assignee, Range&& extent_)
: assignee(std::move(assignee)), extent(extent_) {
}

CallData::CallData(Range&& extent_) : extent(extent_) {
}

bool CallData::requiresDeclaration() const noexcept {
  return assignee.has_value() && !assignee->type.empty();
}

}  // namespace ClangExpand
