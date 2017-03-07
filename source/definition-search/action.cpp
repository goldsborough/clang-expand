// Project includes
#include "clang-expand/definition-search/action.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/definition-search/consumer.hpp"

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
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

Action::Action(const std::string& declarationFile, Query* query)
: _declarationFile(makeAbsolute(declarationFile))
, _query(query) {
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef filename) {
  // Skip the file we found the declaration in
  if (filename == _declarationFile) return nullptr;
  return std::make_unique<Consumer>(_query);
}

}  // namespace ClangExpand::DefinitionSearch
