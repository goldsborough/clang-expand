// Library includes
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/definition-search/action.hpp"

// Clang includes
#include "clang/Frontend/FrontendAction.h"

// Standard includes
#include <string>

namespace ClangExpand::DefinitionSearch {
ToolFactory::ToolFactory(const std::string& declarationFile,
                         const DeclarationState& declaration,
                         const ClangExpand::StateCallback& callback)
: _declaration(declaration)
, _callback(callback)
, _declarationFile(declarationFile) {
}

clang::FrontendAction* ToolFactory::create() {
  return new DefinitionSearch::Action(_declarationFile,
                                      _declaration,
                                      _callback);
}

}  // namespace ClangExpand::DefinitionSearch
