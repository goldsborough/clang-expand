#ifndef CLANG_EXPAND_RESULT_HPP
#define CLANG_EXPAND_RESULT_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/Support/YAMLTraits.h>

// Standard includes
#include <iosfwd>
#include <optional>

namespace ClangExpand {
struct Query;

struct Result {
  explicit Result(Query&& query);
  std::optional<Range> replaceRange;
  std::optional<DeclarationData> declaration;
  std::optional<DefinitionData> definition;
};

llvm::raw_ostream& operator<<(llvm::raw_ostream& stream, const Result& result);
}  // namespace ClangExpand

namespace llvm::yaml {
template <>
struct MappingTraits<ClangExpand::Result> {
  static void mapping(llvm::yaml::IO& io, ClangExpand::Result& result) {
    if (result.replaceRange) {
      io.mapRequired("replace-range", *result.replaceRange);
    }
    if (result.declaration) {
      io.mapRequired("declaration", *result.declaration);
    }
    if (result.definition) {
      io.mapRequired("definition", *result.definition);
    }
  }
};
}  // namespace llvm::yaml

#endif  // CLANG_EXPAND_RESULT_HPP
