#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Project includes
#include "clang-expand/common/state.hpp"
#include "clang-expand/common/structures.hpp"

// Clang includes
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <functional>

namespace clang {
class FrontendAction;
}

namespace ClangExpand::SymbolSearch {

/// A custom \c FrontendActionFactory so that we can pass the options
/// to the constructor of the tool.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  explicit ToolFactory(llvm::StringRef filename,
                       unsigned line,
                       unsigned column,
                       const ClangExpand::StateCallback& callback);

  clang::FrontendAction* create() override;

 private:
  Structures::EasyLocation _targetLocation;
  ClangExpand::StateCallback _callback;
};
}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
