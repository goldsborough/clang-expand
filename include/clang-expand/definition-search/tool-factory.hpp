#ifndef CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP

// Project includes
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

// Standard includes
#include <iosfwd>

namespace ClangExpand::DefinitionSearch {

class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(const std::string& declarationFile,
                       const DeclarationData& declaration,
                       const QueryCallback& callback);

  clang::FrontendAction* create() override;

 private:
  const DeclarationData& _declaration;
  QueryCallback _callback;
  const std::string& _declarationFile;
};
}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
