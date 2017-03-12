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
Result::Result(Query&& query)
: replaceRange(std::move(query.call->extent))
, declaration(std::move(query.declaration))
, definition(std::move(*query.definition)) {
  assert(query.call.has_value() && "Query result has no call data!");
  assert(query.definition.has_value() &&
         "Query result has no definition data!");
}
}  // namespace ClangExpand
