// Project includes
#include "clang-expand/result.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/location.hpp"

// LLVM includes
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <cassert>
#include <string>

namespace ClangExpand {
Result::Result(Query&& query)
: callExtent(std::move(query.call->extent))
, declaration(std::move(*query.declaration))
, definition(std::move(*query.definition)) {
  assert(query.call.has_value() && "Query result has no call data!");
  assert(query.declaration.has_value() &&
         "Query result has no declaration data!");
  assert(query.definition.has_value() &&
         "Query result has no definition data!");
}
}  // namespace ClangExpand
