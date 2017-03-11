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
: call(std::move(*query.call))
, declaration(std::move(*query.declaration))
, definition(std::move(*query.definition)) {
  assert(query.call.has_value() && "Query result has no call data!");
  assert(query.declaration.has_value() &&
         "Query result has no declaration data!");
  assert(query.definition.has_value() &&
         "Query result has no definition data!");
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& stream, const Result& result) {
  stream << result.definition.original << '\n';
  return stream;
}

// std::string Result::toString() const {
//   return
// clang-format off
  // return (llvm::Twine("{\n") +
  //   R"(  "call-location": ")"        + call.toJSON()        + "\",\n"
  //   R"(  "declaration-location": ")" + declaration.toJSON() + "\",\n" +
  //   R"(  "definition-location": ")"  + definition.toJSON()  + "\",\n" +
  //   R"(  "definition": ")"           + definition.original  + "\",\n" +
  //   R"(  "rewritten": ")"            + definition.rewritten + "\"\n}"
  // ).str();
// clang-format on
// }
}  // namespace ClangExpand
