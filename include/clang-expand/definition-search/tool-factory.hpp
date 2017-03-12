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

namespace ClangExpand::DefinitionSearch {

class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(const std::string& declarationFile, Query& _query);

  clang::FrontendAction* create() override;

 private:
  const std::string& _declarationFile;
  Query& _query;
};
}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
