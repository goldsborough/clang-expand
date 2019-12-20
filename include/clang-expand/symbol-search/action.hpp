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

#ifndef CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP

// Project includes
#include "clang-expand/common/location.hpp"

// Clang includes
#include <clang/Basic/SourceLocation.h>
#include <clang/Frontend/FrontendAction.h>

// Standard includes
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
namespace SymbolSearch {

/// \ingroup SymbolSearch
///
/// The `SymbolSearch::Action` class has a major responsibility at the very
/// beginning of the entire `clang-expand` tool.
///
/// It does three main things:
///
/// 1. Translates the location that was specified when invoking clang-expand to
/// a `clang::SourceLocation`, so that it can be used to interact with the rest
/// of clang in further stages of symbol search.
/// 2. Installs preprocessor callbacks to facilitate the part of clang-expand
/// dealing with macros.
/// 3. Returns a `SymbolSearch::Consumer` to continue the processing pipeline.
///
/// Step (1) is one of the more fragile stages of clang-expand as we are dealing
/// with raw (stupid) lexing. Once we have an `ASTConsumer`, things are a bit
/// smoother as we have actual representations inside the AST.
class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;

  /// Constructor, taking the location at which to look for a function call and
  /// the ongoing `Query` object.
  Action(Location targetLocation, Query& query);

  /// Attempts to translate the `targetLocation` to a `clang::SourceLocation`
  /// and install preprocessor hooks for macros.
  bool BeginSourceFileAction(clang::CompilerInstance& compiler) override;

  /// \returns a `SymbolSearch::Consumer`.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  /// Given a `clang::CompilerInstance`, installs appropriate preprocessor
  /// hooks for macro search (looking for macros with the name of the target
  /// function) with the `CompilerInstance`.
  void _installMacroFacilities(clang::CompilerInstance& compiler) const;

  /// The spelling (name/string) of the token under the cursor.
  std::string _spelling;

  /// The ongoing `Query` object.
  Query& _query;

  /// The location under the user's cursor (or what clang-expand was invoked
  /// with).
  Location _targetLocation;

  /// The target location, translated to a `clang::SourceLocation` once we have
  /// found it. We have to store it as a member to be able to pass it to the
  /// `Consumer` inside `CreateASTConsumer`
  clang::SourceLocation _callLocation;
};

}  // namespace SymbolSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP
