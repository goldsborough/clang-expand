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

#ifndef CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP

// Project includes
#include "clang-expand/common/location.hpp"

// Third party includes
#include <third-party/json.hpp>

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

  /// Converts the `DefinitionData` to JSON.
  nlohmann::json toJson() const;

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

#endif  // CLANG_EXPAND_COMMON_DEFINITION_DATA_HPP
