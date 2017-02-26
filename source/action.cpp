// Project includes
#include "clang-expand/action.hpp"

// Clang includes
#include "clang/Frontend/CompilerInstance.h"

// Standard includes
#include <string>

namespace ClangExpand {

Action::Action(const std::string& filename, unsigned line, unsigned column)
: _filename(filename), _line(line), _column(column) {
}

bool Action::BeginInvocation(clang::CompilerInstance& compiler) {
  return false;
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance& compiler,
                          llvm::StringRef filename) {
  if (int x = 5; x == 5) {
    std::string s = std::to_string(x);
  }

  return nullptr;
}

}  // namespace ClangExpand
