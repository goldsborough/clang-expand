#ifndef CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP

// Project includes
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/Support/YAMLTraits.h>

// Standard includes
#include <string>

namespace clang {
class FunctionDecl;
class ASTContext;
}

namespace ClangExpand {
class Query;

struct DefinitionData {
  static DefinitionData Collect(const clang::FunctionDecl& function,
                                clang::ASTContext& context,
                                const Query& query);
  Location location;
  std::string original;
  std::string rewritten;
  bool isMacro{false};
};
}  // namespace ClangExpand

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::DefinitionData> {
  static void
  mapping(llvm::yaml::IO& io, ClangExpand::DefinitionData& definition) {
    io.mapRequired("location", definition.location);
    io.mapRequired("isMacro", definition.isMacro);
    io.mapRequired("text", definition.original);
    if (!definition.rewritten.empty()) {
      io.mapOptional("rewritten", definition.rewritten);
    }
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
