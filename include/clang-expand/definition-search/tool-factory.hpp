#ifndef CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/Tooling/Tooling.h"

// Standard includes
#include <string>

namespace clang {
class FrontendAction;
}

namespace ClangExpand::DefinitionSearch {

class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(const std::string& declarationFile,
                       const DeclarationState& declaration,
                       const StateCallback& callback);

  clang::FrontendAction* create() override;

 private:
  const DeclarationState& _declaration;
  StateCallback _callback;
  const std::string& _declarationFile;
};
}  // namespace ClangExpand::DefinitionSearch

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_TOOL_FACTORY_HPP
