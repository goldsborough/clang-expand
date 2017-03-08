// Project includes
#include "clang-expand/common/assignee-data.hpp"

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {
AssigneeData::AssigneeData(const std::string& name_)
: AssigneeData("=", name_) {
}

AssigneeData::AssigneeData(const llvm::StringRef& op_,
                           const std::string& name_,
                           const std::string& type_)
: op(op_), name(name_), type(type_) {
}
}  // namespace ClangExpand
