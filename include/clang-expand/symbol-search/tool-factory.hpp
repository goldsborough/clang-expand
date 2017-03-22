#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

namespace ClangExpand {
struct Query;
struct Location;
}

namespace ClangExpand { namespace SymbolSearch {

/// \ingroup SymbolSearch
///
/// Simple factory class to create a parameterized `SymbolSearch` tool.
///
/// This class is required because the standard `newFrontendAction` function
/// does not allow passing parameters to an action.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  /// Constructor, taking the location the user invoked clang-expand with and
  /// the fresh `Query` object.
  explicit ToolFactory(const Location& _targetLocation, Query& query);

  /// Creates the action of the symbol search phase.
  /// \returns A `SymbolSearch::Action`.
  clang::FrontendAction* create() override;

 private:
  /// The location at which the user invoked clang-expand.
  const Location& _targetLocation;

  /// The newly created `Query` object.
  Query& _query;
};
}  // namespace SymbolSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
