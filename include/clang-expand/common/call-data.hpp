#ifndef CLANG_EXPAND_COMMON_CALL_DATA_HPP
#define CLANG_EXPAND_COMMON_CALL_DATA_HPP

// Project includes
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/structures.hpp"

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {
struct CallData {
  explicit CallData(Range&& extent_);
  CallData(AssigneeData&& assignee, Range&& extent_);

  bool requiresDeclaration() const noexcept;

  std::optional<AssigneeData> assignee;
  Range extent;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_CALL_DATA_HPP
