#ifndef CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

// Standard includes
#include <iosfwd>

namespace ClangExpand {
struct Query;
}

namespace ClangExpand {
namespace DefinitionSearch {

/// \ingroup DefinitionSearch
///
/// Simple factory class to create a parameterized `DefinitionSearch` tool.
///
/// This class is required because the standard `newFrontendAction` function
/// does not allow passing parameters to an action.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  /// Constructor, taking the file in which the declaration was found and the
  /// ongoing `Query`. This tool will skip the `declarationFile`, since its
  /// definition would already have been picked up during symbol search, if it
  /// had one.
  explicit ToolFactory(const std::string& declarationFile, Query& query);

  /// Creates the action of the definition search phase.
  /// \returns A `DefinitionSearch::Action`.
  clang::FrontendAction* create() override;

 private:
  /// The file in which the declaration was found.
  const std::string& _declarationFile;

  /// The ongoing `Query` object.
  Query& _query;
};
}  // namespace DefinitionSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
