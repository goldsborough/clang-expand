#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

namespace ClangExpand {
class Query;
struct Location;
}

namespace ClangExpand::SymbolSearch {
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(const Location& _targetLocation, Query& query);

  clang::FrontendAction* create() override;

 private:
  const Location& _targetLocation;
  Query& _query;
};
}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
