#ifndef CLANG_EXPAND_SYMBOL_SEARCH_FUNCTION_PROPERTIES_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_FUNCTION_PROPERTIES_HPP

// LLVM includes
#include "llvm/ADT/SmallVector.h"

// Standard includes
#include <string>

namespace ClangExpand {
struct FunctionProperties {
  llvm::SmallVector<std::string, 8> namespaces;
  unsigned numberOfArguments;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_FUNCTION_PROPERTIES_HPP
