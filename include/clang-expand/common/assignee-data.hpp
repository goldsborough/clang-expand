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

  struct Type {
    Type(const std::string& name_ = std::string(),
         bool isDefaultConstructible_ = true) noexcept;
    std::string name;
    bool isDefaultConstructible;
  };

  class Builder {
   public:
    explicit Builder(AssigneeData&& assignee = AssigneeData());
    Builder(const Builder&) = delete;
    Builder& operator=(const Builder&) = delete;

    Builder& name(const llvm::StringRef& name);
    Builder& type(const llvm::StringRef& name,
                  bool isDefaultConstructible = true);
    Builder& op(const llvm::StringRef& op);

    AssigneeData build();

   private:
    AssigneeData&& _assignee;
  };

  OperatorString op;
  std::string name;
  std::optional<Type> type;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_ASSIGNEE_DATA_HPP
