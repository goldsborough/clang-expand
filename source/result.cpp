// Project includes
#include "clang-expand/result.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"

// LLVM includes
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <cassert>
#include <optional>
#include <string>

namespace ClangExpand {
Result::Result(Query&& query) {
  if (query.options.wantsCall) {
    assert(query.call.has_value() &&
           "User wants call information, but no call data.");
    replaceRange = std::move(query.call->extent);
  }
  if (query.options.wantsDeclaration) {
    declaration = std::move(query.declaration);
  }
  if (query.requiresDefinition()) {
    definition = std::move(query.definition);
  }
}
}  // namespace ClangExpand
