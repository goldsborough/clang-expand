//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

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
  auto trimmed = name.rtrim();
  _assignee.name.assign(trimmed.begin(), trimmed.end());
  return *this;
}

AssigneeData::Builder& AssigneeData::Builder::type(
    const llvm::StringRef& name, bool isDefaultConstructible) {
  _assignee.type.emplace(name.str(), isDefaultConstructible);
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
  if (withType && type.hasValue()) {
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
