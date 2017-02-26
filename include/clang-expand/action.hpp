#ifndef CLANG_EXPAND_TOOL_HPP
#define CLANG_EXPAND_TOOL_HPP

// Clang includes
#include "clang/Frontend/FrontendAction.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <memory>
#include <string>

namespace clang {
class CompilerInstance;
}

namespace ClangExpand {

class Action : public clang::ASTFrontendAction {
 public:
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  Action(const std::string& filename, unsigned line, unsigned column);

  bool BeginInvocation(clang::CompilerInstance& compiler) override;

  /// Creates the Consumer instance, forwarding the command line options.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  std::string _filename;
  unsigned _line;
  unsigned _column;
};

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_TOOL_HPP
