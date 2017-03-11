// Project includes
#include "clang-expand/symbol-search/tool-factory.hpp"
#include "clang-expand/symbol-search/action.hpp"
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>

namespace ClangExpand::SymbolSearch {
ToolFactory::ToolFactory(const Location& targetLocation, Query& query)
: _targetLocation(targetLocation), _query(query) {
}

clang::FrontendAction* ToolFactory::create() {
  return new SymbolSearch::Action(_targetLocation, _query);
}

}  // namespace ClangExpand::SymbolSearch
