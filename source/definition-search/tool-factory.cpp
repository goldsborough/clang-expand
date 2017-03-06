// Library includes
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/definition-search/action.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>

namespace ClangExpand::DefinitionSearch {
ToolFactory::ToolFactory(const std::string& declarationFile,
                         const DeclarationData& declaration,
                         const ClangExpand::QueryCallback& callback)
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
