// Project includes
#include "clang-expand/result.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"

// Third party includes
#include <third-party/json.hpp>

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

  if (callRange.has_value()) {
    json["call"] = callRange->toJson();
  }

  if (declaration.has_value()) {
    json["declaration"] = declaration->toJson();
  }

  if (definition.has_value()) {
    json["definition"] = definition->toJson();
  }

  return json.is_null() ? "" : json;
}

}  // namespace ClangExpand
