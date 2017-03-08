#ifndef CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP
#define CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {
struct AssigneeData {
  // <<=/>>= should be the longest compound assignment.
  using OperatorString = llvm::SmallString<3>;

  explicit AssigneeData(const std::string& name_);
  AssigneeData(const llvm::StringRef& op_,
               const std::string& name_,
               const std::string& type_ = std::string());

  OperatorString op;
  std::string name;
  std::string type;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP
