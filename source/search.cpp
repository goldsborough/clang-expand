// Library includes
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

int Search::run(clang::tooling::CompilationDatabase& compilationDatabase,
                const SourceVector& sources) {
  _state.reset();

  if (auto error = _symbolSearch(compilationDatabase); error) {
    return error;
  }

  if (_state->isEmpty()) {
    llvm::outs() << "Found nothing at all" << '\n';
    return EXIT_FAILURE;
  }

  if (_state->hasCall()) {
    const auto& call = _state->call();
    llvm::outs() << "Found call information: At " << call.extent.begin.line
                 << ':' << call.extent.begin.column << " - "
                 << call.extent.end.line << ':' << call.extent.end.column;

    if (call.variable) {
      llvm::outs() << ": type = " << call.variable->type
                   << ", name = " << call.variable->name;
    }

    llvm::outs() << '\n';
  }

  if (_state->isDefinition()) {
    llvm::outs() << _state->definition().code << '\n';
    return EXIT_SUCCESS;
  }

  if (auto error = _definitionSearch(compilationDatabase, sources); error) {
    return error;
  }

  if (_state->isDefinition()) {
    llvm::outs() << _state->definition().code << '\n';
    return EXIT_SUCCESS;
  }

  return EXIT_FAILURE;
  // return optional<definition state> instead of error code and print in main
}

int Search::_symbolSearch(CompilationDatabase& compilationDatabase) {
  clang::tooling::ClangTool SymbolSearch(compilationDatabase,
                                         {_location.filename});

  // clang-format off
  return SymbolSearch.run(new ClangExpand::SymbolSearch::ToolFactory(
    _location, [this] (auto&& result) {
        _state = std::move(result);
    }));
  // clang-format on
}

int Search::_definitionSearch(CompilationDatabase& compilationDatabase,
                              const SourceVector& sources) {
  clang::tooling::ClangTool DefinitionSearch(compilationDatabase, sources);

  // clang-format off
  return DefinitionSearch.run(new ClangExpand::DefinitionSearch::ToolFactory(
      _location.filename, _state->declaration(),
      [this](auto&& result) {
        _state = std::move(result);
    }));
  // clang-format on
}

}  // namespace ClangExpand
