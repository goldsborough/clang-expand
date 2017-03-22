//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

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
