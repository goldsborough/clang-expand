// Project includes
#include "clang-expand/definition-search/action.hpp"
#include "clang-expand/definition-search/consumer.hpp"

#include <string>

namespace ClangExpand::DefinitionSearch {

Action::Action(const std::string& declarationFile,
               const DeclarationState& declaration,
               const StateCallback& stateCallback)
: _declarationFile(declarationFile)
, _declaration(declaration)
, _stateCallback(stateCallback) {
}

bool Action::BeginSourceFileAction(clang::CompilerInstance& compiler,
                                   llvm::StringRef filename) {
  if (!super::BeginSourceFileAction(compiler, filename)) return false;
  if (filename == _declarationFile) return false;
  return true;
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) {
  return std::make_unique<Consumer>(_declaration, _stateCallback);
}

}  // namespace ClangExpand::DefinitionSearch
