// Project includes
#include "clang-expand/result.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/options.hpp"

// Third party includes
#include <third-party/json.hpp>

// LLVM includes
#include <llvm/ADT/Optional.h>

// Standard includes
#include <cassert>
#include <utility>

namespace ClangExpand {
Result::Result(Query&& query) {
  if (query.options.wantsCall) {
    assert(query.call.hasValue() &&
           "User wants call information, but have no call data.");
    callRange = query.call->extent;
  }
  if (query.options.wantsDeclaration) {
    declaration = std::move(query.declaration);
  }
  if (query.requiresDefinition()) {
    definition = std::move(query.definition);
  }
}

nlohmann::json Result::toJson() const {
  nlohmann::json json;

  if (callRange.hasValue()) {
    json["call"] = callRange->toJson();
  }

  if (declaration.hasValue()) {
    json["declaration"] = declaration->toJson();
  }

  if (definition.hasValue()) {
    json["definition"] = definition->toJson();
  }

  return json.is_null() ? "" : json;
}

}  // namespace ClangExpand
