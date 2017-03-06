#ifndef CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP

// Project includes
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>

// Standard includes
#include <iosfwd>
#include <memory>
#include <string>

namespace clang {
class CompilerInstance;
class ASTConsumer;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand::DefinitionSearch {

class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  Action(const std::string& declarationFile,
         const DeclarationData& declaration,
         const QueryCallback& stateCallback);

  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  const std::string _declarationFile;
  const DeclarationData& _declaration;
  QueryCallback _stateCallback;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
