#ifndef CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
#define CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP

// Project includes
#include "clang-expand/common/context-data.hpp"
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace ClangExpand {
using ContextDataVector = llvm::SmallVector<ContextData, 8>;
using TypeVector = llvm::SmallVector<std::string, 8>;
using ParameterMap = llvm::StringMap<std::string>;

struct DeclarationData {
  explicit DeclarationData(const llvm::StringRef& name_,
                           const Location& location_)
  : name(name_.str()), location(location_) {
  }

  std::string name;
  ContextDataVector contexts;
  TypeVector parameterTypes;
  ParameterMap parameterMap;
  Location location;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
