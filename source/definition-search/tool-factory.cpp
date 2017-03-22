// Project includes
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/definition-search/action.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>

// Standard includes
#include <string>

namespace ClangExpand {
namespace DefinitionSearch {
ToolFactory::ToolFactory(const std::string& declarationFile, Query& query)
: _declarationFile(declarationFile), _query(query) {
}

clang::FrontendAction* ToolFactory::create() {
  return new DefinitionSearch::Action(_declarationFile, _query);
}

}  // namespace DefinitionSearch
}  // namespace ClangExpand
