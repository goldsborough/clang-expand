#ifndef CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP

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

namespace ClangExpand {
struct Query;
}

namespace ClangExpand::DefinitionSearch {

class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  Action(const std::string& declarationFile, Query& query);

  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  std::string _declarationFile;
  Query& _query;
};

}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
