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

#ifndef CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
#define CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP

// Clang includes
#include <clang/AST/DeclBase.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace ClangExpand {

/// Stores information about a context (namespace, class name etc.).
///
/// C++ allows classes inside namespaces to have the same name as the namespace
/// (`namespace X { struct X {}; }`), so we can't just store the name, we also
/// need the `Decl::Kind`.
struct ContextData {
  /// Constructor.
  ContextData(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_.str()) {
  }

  /// The `clang::Decl::Kind` of the context (usually CXXRecord or Namespace).
  clang::Decl::Kind kind;

  /// The name of the context (class, namespace etc.).
  std::string name;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
