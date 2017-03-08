// Project includes
#include "clang-expand/common/assignee-data.hpp"

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {

AssigneeData::Builder::Builder(AssigneeData&& assignee)
: _assignee(std::move(assignee)) {
}

AssigneeData::Builder&
AssigneeData::Builder::name(const llvm::StringRef& name) {
  _assignee.name = name;
  return *this;
}

AssigneeData::Builder&
AssigneeData::Builder::type(const llvm::StringRef& type) {
  _assignee.type = type;
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::op(const llvm::StringRef& op) {
  _assignee.op = op;
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::defaultConstructible(bool yes) {
  _assignee.isDefaultConstructible = yes;
  return *this;
}

AssigneeData AssigneeData::Builder::build() {
  return std::move(_assignee);
}
}  // namespace ClangExpand
