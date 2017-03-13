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
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <cstdlib>
#include <optional>
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
