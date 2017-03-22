// Project includes
#include "clang-expand/common/assignee-data.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>

// Standard includes
#include <cassert>
#include <string>
#include <type_traits>

namespace ClangExpand {

AssigneeData::Type::Type(std::string name_,
                         bool isDefaultConstructible_) noexcept
: name(std::move(name_)), isDefaultConstructible(isDefaultConstructible_) {
}

AssigneeData::Builder::Builder(AssigneeData&& assignee)
: _assignee(std::move(assignee)) {
}

AssigneeData::Builder&
AssigneeData::Builder::name(const llvm::StringRef& name) {
  _assignee.name = name.rtrim();
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::type(
    const llvm::StringRef& name, bool isDefaultConstructible) {
  _assignee.type.emplace(name, isDefaultConstructible);
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
  return !type.hasValue() || type->isDefaultConstructible;
}

std::string AssigneeData::toAssignment(bool withType) const {
  if (withType) {
    assert(type.hasValue() &&
           "Requested assignee string with type, but have no type");
    return (llvm::Twine(type->name) + " " + name + " " + op).str();
  }
  return (llvm::Twine(name) + " " + op).str();
}

std::string AssigneeData::toDeclaration() const {
  assert(type.hasValue() && "Requested assignee declaration, but have no type");
  return (llvm::Twine(type->name) + " " + name + ";").str();
}

}  // namespace ClangExpand
