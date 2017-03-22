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
#include "clang-expand/search.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/result.hpp"
#include "clang-expand/symbol-search/tool-factory.hpp"

// Clang includes
#include <clang/Tooling/Tooling.h>

// LLVM includes
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <cstdlib>
#include <string>
#include <type_traits>

namespace ClangExpand {
Search::Search(const std::string& file, unsigned line, unsigned column)
: _location(Routines::makeAbsolute(file), line, column) {
}

Result Search::run(clang::tooling::CompilationDatabase& compilationDatabase,
                   const SourceVector& sources,
                   const Options& options) {
  Query query(options);

  _symbolSearch(compilationDatabase, query);

  if (query.foundNothing()) {
    Routines::error("Could not recognize token at specified location");
  }

  if (query.requiresDefinition()) {
    if (!query.definition) {
      _definitionSearch(compilationDatabase, sources, query);
    }

    if (!query.definition) {
      Routines::error("Could not find definition");
    }
  }

  return Result(std::move(query));
}

void Search::_symbolSearch(CompilationDatabase& compilationDatabase,
                           Query& query) {
  clang::tooling::ClangTool SymbolSearch(compilationDatabase,
                                         {_location.filename});

  const auto error = SymbolSearch.run(
      new ClangExpand::SymbolSearch::ToolFactory(_location, query));
  if (error) std::exit(error);
}

void Search::_definitionSearch(CompilationDatabase& compilationDatabase,
                               const SourceVector& sources,
                               Query& query) {
  clang::tooling::ClangTool DefinitionSearch(compilationDatabase, sources);

  const auto error = DefinitionSearch.run(
      new ClangExpand::DefinitionSearch::ToolFactory(_location.filename,
                                                     query));
  if (error) std::exit(error);
}

}  // namespace ClangExpand
