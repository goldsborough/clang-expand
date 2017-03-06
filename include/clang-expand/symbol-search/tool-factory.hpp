#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

namespace ClangExpand {
namespace Structures {
struct EasyLocation;
}
}

namespace ClangExpand::SymbolSearch {

/// A custom \c FrontendActionFactory so that we can pass the options
/// to the constructor of the tool.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(const Structures::EasyLocation& _targetLocation,
                       const ClangExpand::StateCallback& callback);

  clang::FrontendAction* create() override;

 private:
  const Structures::EasyLocation& _targetLocation;
  ClangExpand::StateCallback _callback;
};
}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
