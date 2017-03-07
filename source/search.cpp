// Project includes
#include "clang-expand/search.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/symbol-search/tool-factory.hpp"

// Clang includes
#include <clang/Tooling/Tooling.h>

// Standard includes
#include <cstdlib>
#include <functional>
#include <string>
#include <variant>
#include <vector>

namespace ClangExpand {

Search::Search(const std::string& file, unsigned line, unsigned column)
: _location(file, line, column) {
}

Search::ResultOrError
Search::run(clang::tooling::CompilationDatabase& compilationDatabase,
            const SourceVector& sources) {
  Query query;

  if (auto error = _symbolSearch(compilationDatabase, query); error) {
    return error;
  }

  llvm::outs() << "decl: " << query.isDeclaration()
               << " def: " << query.isDefinition() << '\n';

  if (!query) {
    llvm::outs() << "Found nothing at all" << '\n';
    return EXIT_FAILURE;
  }

  if (const auto& call = query.call(); call) {
    llvm::outs() << "Found call information: At " << call->extent.begin.line
                 << ':' << call->extent.begin.column << " - "
                 << call->extent.end.line << ':' << call->extent.end.column;

    if (call->variable) {
      llvm::outs() << ": type = " << call->variable->type
                   << ", name = " << call->variable->name;
    }

    llvm::outs() << '\n';
  }

  if (query.isDefinition()) {
    llvm::outs() << query.definition().code << '\n';
    return EXIT_SUCCESS;
  }

  const auto error = _definitionSearch(compilationDatabase, sources, query);
  if (error) return error;

  if (query.isDefinition()) {
    llvm::outs() << query.definition().code << '\n';
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
}

int Search::_symbolSearch(CompilationDatabase& compilationDatabase,
                          Query& query) {
  clang::tooling::ClangTool SymbolSearch(compilationDatabase,
                                         {_location.filename});

  return SymbolSearch.run(
      new ClangExpand::SymbolSearch::ToolFactory(_location, &query));
}

int Search::_definitionSearch(CompilationDatabase& compilationDatabase,
                              const SourceVector& sources,
                              Query& query) {
  clang::tooling::ClangTool DefinitionSearch(compilationDatabase, sources);

  return DefinitionSearch.run(
      new ClangExpand::DefinitionSearch::ToolFactory(_location.filename,
                                                     &query));
}

}  // namespace ClangExpand
