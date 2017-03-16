// Project includes
#include "clang-expand/common/assignee-data.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>

// Standard includes
#include <cassert>
#include <optional>
#include <string>
#include <type_traits>

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
  _assignee.name = name.rtrim();
  return *this;
}

AssigneeData::Builder&
AssigneeData::Builder::type(const llvm::StringRef& type,
                            bool isDefaultConstructible) {
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

std::string AssigneeData::toString(bool withType) const {
  if (withType) {
    assert(type.has_value() &&
           "Requested assignee string with type, but have no type");
    return (llvm::Twine(type->name) + " " + name + " " + op).str();
  }
  return (llvm::Twine(name) + " " + op).str();
}

}  // namespace ClangExpand
