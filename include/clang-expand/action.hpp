#ifndef CLANG_EXPAND_TOOL_HPP
#define CLANG_EXPAND_TOOL_HPP

// Clang includes
#include "clang/Frontend/FrontendAction.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <memory>

namespace ClangExpand {

class Action : public clang::ASTFrontendAction {
 public:
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  /// Creates the Consumer instance, forwarding the command line options.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& Compiler,
                                       llvm::StringRef Filename) override;
};

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_TOOL_HPP
