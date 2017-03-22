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

#ifndef CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
#define CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP

// Project includes
#include "clang-expand/common/context-data.hpp"
#include "clang-expand/common/location.hpp"

// Third party includes
#include <third-party/json.hpp>

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>

// Standard includes
#include <string>

namespace ClangExpand {
/// Stores information about a function declaration.
///
/// This information is used to uniquely identify any function that we might
/// look for. Note that the function could actually be a constructor or operator
/// overload.
struct DeclarationData {
  using ParameterMap = llvm::StringMap<std::string>;

  /// Constructor.
  explicit DeclarationData(std::string name_, Location location_);

  /// Converts the `DeclarationData` to JSON.
  nlohmann::json toJson() const;

  /// The name of the function (or operator).
  std::string name;

  /// The raw source text of the entire function declaration.
  ///
  /// If the declaration is also a definition, this will include the definition.
  std::string text;

  /// The contexts of the function (namespaces, class names etc.).
  ///
  /// The order of these contexts is from most nested to least nested, i.e.
  /// given a function, its contexts can be compared from "the inside out". For
  /// example, given this function definition:
  ///
  /// ```.cpp
  /// namesapce A {
  /// namespace B {
  /// class C {
  ///   void f();
  /// };
  /// }
  /// }
  /// ```
  ///
  /// The contexts will be 'C', 'B', 'A' (in this order!).
  llvm::SmallVector<ContextData, 8> contexts;

  /// The types of the parameters as they appear in the function definition.
  ///
  /// The type strings stored are fully qualified to retain as much information
  /// as possible across for serialization between symbol and definition search.
  /// More precisely, the type information is retrieved through:
  /// `parameter->getOriginalType().getCanonicalType().getAsString(policy)`,
  /// where `parameter` is a `ParmVarDecl` and `policy` is an appropriate
  /// `PrintingPolicy` retrieved from the `ASTContext` that stores the full
  /// qualification of the name.
  llvm::SmallVector<std::string, 8> parameterTypes;

  /// The mapping from parameter names to respective argument expressions.
  ///
  /// For example, given this function and call:
  /// ```.cpp
  /// void f(int x, double y);
  /// f(1, g(2));
  /// ```
  ///
  /// where `g` is some double-valued function, the mapping will be:
  /// - x -> 1
  /// - y -> g(2)
  /// such that during rewriting, every ocurrence of `x` and `y` inside the
  /// function can be replaced with `1` and `g(2)`, respectively.
  ParameterMap parameterMap;

  /// The location of the function declaration (right at the name of the
  /// function).
  Location location;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DECLARATION_DATA_HPP
