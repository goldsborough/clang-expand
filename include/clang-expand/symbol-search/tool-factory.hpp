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

#ifndef CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP

// Clang includes
#include <clang/Frontend/FrontendAction.h>
#include <clang/Tooling/Tooling.h>

namespace ClangExpand {
struct Query;
struct Location;
}

namespace ClangExpand {
namespace SymbolSearch {

/// \ingroup SymbolSearch
///
/// Simple factory class to create a parameterized `SymbolSearch` tool.
///
/// This class is required because the standard `newFrontendAction` function
/// does not allow passing parameters to an action.
class ToolFactory : public clang::tooling::FrontendActionFactory {
 public:
  /// Constructor, taking the location the user invoked clang-expand with and
  /// the fresh `Query` object.
  explicit ToolFactory(const Location& _targetLocation, Query& query);

  /// Creates the action of the symbol search phase.
  /// \returns A `SymbolSearch::Action`.
  std::unique_ptr<clang::FrontendAction> create() override;

 private:
  /// The location at which the user invoked clang-expand.
  const Location& _targetLocation;

  /// The newly created `Query` object.
  Query& _query;
};
}  // namespace SymbolSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_TOOL_FACTORY_HPP
