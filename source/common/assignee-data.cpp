// Project includes
#include "clang-expand/common/assignee-data.hpp"

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {

AssigneeData::Type::Type(const std::string& name_,
                         bool isDefaultConstructible_) noexcept
: name(name_), isDefaultConstructible(isDefaultConstructible_) {
}

AssigneeData::Builder::Builder(AssigneeData&& assignee)
: _assignee(std::move(assignee)) {
}

AssigneeData::Builder&
AssigneeData::Builder::name(const llvm::StringRef& name) {
  _assignee.name = name;
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::type(
    const llvm::StringRef& type, bool isDefaultConstructible) {
  _assignee.type.emplace(type, isDefaultConstructible);
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::op(const llvm::StringRef& op) {
  _assignee.op = op;
  return *this;
}

AssigneeData AssigneeData::Builder::build() {
  return std::move(_assignee);
}

bool AssigneeData::isDefaultConstructible() const noexcept {
  return !type.has_value() || type->isDefaultConstructible;
}

}  // namespace ClangExpand
