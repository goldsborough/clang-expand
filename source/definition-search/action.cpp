// Project includes
#include "clang-expand/definition-search/action.hpp"
#include "clang-expand/definition-search/consumer.hpp"

// LLVM includes
#include <llvm/Support/FileSystem.h>
#include <llvm/ADT/SmallString.h>

// Standard includes
#include <string>
#include <cassert>
#include <system_error>

namespace ClangExpand::DefinitionSearch {
namespace {
std::string makeAbsolute(const std::string& filename) {
  llvm::SmallString<256> absolutePath(filename);
  const auto error = llvm::sys::fs::make_absolute(absolutePath);
  assert(!error && "Error generating absolute path");
  return absolutePath.str();
}
}  // namespace

Action::Action(const std::string& declarationFile,
               const DeclarationState& declaration,
               const StateCallback& stateCallback)
: _declarationFile(makeAbsolute(declarationFile))
, _declaration(declaration)
, _stateCallback(stateCallback) {
}

bool Action::BeginSourceFileAction(clang::CompilerInstance& compiler,
                                   llvm::StringRef filename) {
  if (!super::BeginSourceFileAction(compiler, filename)) return false;
  llvm::outs() << filename << " = " << _declarationFile <<'\n';
  if (filename == _declarationFile) return false;
  return true;
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) {
  return std::make_unique<Consumer>(_declaration, _stateCallback);
}

}  // namespace ClangExpand::DefinitionSearch
