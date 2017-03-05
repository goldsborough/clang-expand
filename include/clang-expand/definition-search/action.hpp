#ifndef CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Token.h"

// Standard includes
#include <functional>
#include <memory>
#include <string>

namespace clang {
class CompilerInstance;
class FileID;
class SourceManager;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand::DefinitionSearch {

class State;

class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  Action(const std::string& declarationFile,
         const DeclarationState& declaration,
         const StateCallback& stateCallback);

  bool BeginSourceFileAction(clang::CompilerInstance& compiler,
                             llvm::StringRef filename) override;

  /// Creates the Consumer instance, forwarding the command line options.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  const std::string& _declarationFile;
  const DeclarationState& _declaration;
  StateCallback _stateCallback;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
