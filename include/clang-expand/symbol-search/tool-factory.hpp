#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Clang includes
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <functional>

namespace clang {
class FrontendAction;
}

namespace ClangExpand {
struct FunctionProperties;
}

namespace ClangExpand::SymbolSearch {

/// A custom \c FrontendActionFactory so that we can pass the options
/// to the constructor of the tool.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  using ResultCallback =
      std::function<void(const ClangExpand::FunctionProperties&)>;

  explicit ToolFactory(llvm::StringRef filename,
                       unsigned line,
                       unsigned column,
                       const ResultCallback& callback);

  clang::FrontendAction* create() override;

 private:
  llvm::StringRef _filename;
  unsigned _line;
  unsigned _column;
  ResultCallback _callback;
};
}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
