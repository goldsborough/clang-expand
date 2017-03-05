// Project includes
#include "clang-expand/definition-search/action.hpp"
#include "clang-expand/definition-search/consumer.hpp"

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/Support/FileSystem.h>

// Standard includes
#include <cassert>
#include <string>
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

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef filename) {
  if (filename == _declarationFile) return nullptr;
  return std::make_unique<Consumer>(_declaration, _stateCallback);
}

}  // namespace ClangExpand::DefinitionSearch
