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

#ifndef CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP

// Clang includes
#include <clang/Frontend/FrontendAction.h>

// Standard includes
#include <iosfwd>
#include <memory>
#include <string>

namespace clang {
class CompilerInstance;
class ASTConsumer;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand {
struct Query;
}

namespace ClangExpand {
namespace DefinitionSearch {

/// \ingroup DefinitionSearch
///
/// The first entry point into the definition search phase.
///
/// The only real responsibility of this class is to return a `nullptr` when
/// invoked on the declaration file and otherwise the
/// `DefinitionSearch::Consumer` (a `clang::ASTConsumer`).
class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  /// Constructor, taking the file in which the found function was declared and
  /// the ongoing `Query` object. The `declarationFile` is needed because the
  /// `Action` will skip this file, since we already would have found its
  /// definition during symbol search, if it had one.
  Action(const std::string& declarationFile, Query& query);

  /// If the `Action` is invoked on the `declarationFile` argument to the
  /// constructor, returns a `nullptr`. Else returns a
  /// `DefinitionSearch::Consumer` to continue the pipeline.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  /// The file in which the declaration was found. While a `std::string&` would
  /// work, we actually translate the `declarationFile` to an absolute path, so
  /// storing a `std::string` value.
  std::string _declarationFile;

  /// The ongoing `Query` object.
  Query& _query;
};

}  // namespace DefinitionSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_ACTION_HPP
