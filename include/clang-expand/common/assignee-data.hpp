#ifndef CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP
#define CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace ClangExpand {
class AssigneeData {
 public:
  // <<=/>>= should be the longest compound assignment.
  using OperatorString = llvm::SmallString<3>;

  class Builder {
   public:
    explicit Builder(AssigneeData&& assignee = AssigneeData());
    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

    Builder& name(const llvm::StringRef& name);
    Builder& type(const llvm::StringRef& type);
    Builder& op(const llvm::StringRef& op);
    Builder& defaultConstructible(bool yes = true);

    AssigneeData build();

   private:
    AssigneeData&& _assignee;
  };

  bool isDefaultConstructible;
  OperatorString op;
  std::string name;
  std::string type;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP
