#ifndef CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP

// Project includes
#include "clang-expand/common/structures.hpp"

// Standard includes
#include <string>

namespace ClangExpand {
struct DefinitionData {
  std::string toJSON() const { return ""; }

  Location location;
  std::string original;
  std::string rewritten;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
