// Library includes
#include "clang-expand/symbol-search/tool-factory.hpp"
#include "clang-expand/symbol-search/action.hpp"

// Clang includes
#include "clang/Frontend/FrontendAction.h"

namespace ClangExpand::SymbolSearch {
ToolFactory::ToolFactory(llvm::StringRef filename,
                         unsigned line,
                         unsigned column,
                         const ResultCallback& callback)
: _filename(filename), _line(line), _column(column), _callback(callback) {
}

clang::FrontendAction* ToolFactory::create() {
  return new SymbolSearch::Action(_filename, _line, _column, _callback);
}

}  // namespace ClangExpand::SymbolSearch
