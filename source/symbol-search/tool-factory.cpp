// Library includes
#include "clang-expand/symbol-search/tool-factory.hpp"
#include "clang-expand/symbol-search/action.hpp"

// Clang includes
#include "clang/Frontend/FrontendAction.h"

namespace ClangExpand::SymbolSearch {
ToolFactory::ToolFactory(const Structures::EasyLocation& targetLocation,
                         const ClangExpand::StateCallback& callback)
: _targetLocation(targetLocation), _callback(callback) {
}

clang::FrontendAction* ToolFactory::create() {
  return new SymbolSearch::Action(_targetLocation, _callback);
}

}  // namespace ClangExpand::SymbolSearch
