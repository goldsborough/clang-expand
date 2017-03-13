#ifndef CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
#define CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP

// Project includes
#include "clang-expand/common/context-data.hpp"
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/YAMLTraits.h>

// Standard includes
#include <string>

namespace ClangExpand {
struct DeclarationData {
  using ParameterMap = llvm::StringMap<std::string>;

  explicit DeclarationData(const std::string& name_,
                           const Location& location_)
  : name(name_), location(location_) {
  }

  std::string name;
  std::string text;
  llvm::SmallVector<ContextData, 8> contexts;
  llvm::SmallVector<std::string, 8> parameterTypes;
  ParameterMap parameterMap;
  Location location;
};
}  // namespace ClangExpand

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::DeclarationData> {
  static void
  mapping(llvm::yaml::IO& io, ClangExpand::DeclarationData& declaration) {
    io.mapRequired("location", declaration.location);
    io.mapRequired("name", declaration.name);
    io.mapRequired("text", declaration.text);
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
