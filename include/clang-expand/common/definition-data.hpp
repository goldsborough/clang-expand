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
struct Query;

/// Stores data about the definition of a function.
struct DefinitionData {
  /// Collects the entire `DefinitionData` for the function. The `query` passed
  /// should already have been through symbol search and must store `CallData`
  /// and `DeclarationData`.
  static DefinitionData Collect(const clang::FunctionDecl& function,
                                clang::ASTContext& context,
                                const Query& query);
  /// The `Location` of the definition in the source.
  Location location;

  /// The original, untouched source text of the definition.
  std::string original;

  /// The rewritten (expanded) source text of the definition.
  std::string rewritten;

  /// Whether this definition is from a macro or a real function.
  bool isMacro{false};
};
}  // namespace ClangExpand

namespace llvm::yaml {
/// Serialization traits for YAML output.
template <>
struct MappingTraits<ClangExpand::DefinitionData> {
  static void
  mapping(llvm::yaml::IO& io, ClangExpand::DefinitionData& definition) {
    io.mapRequired("location", definition.location);
    io.mapRequired("isMacro", definition.isMacro);
    if (!definition.original.empty()) {
      io.mapRequired("text", definition.original);
    }
    if (!definition.rewritten.empty()) {
      io.mapOptional("rewritten", definition.rewritten);
    }
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
