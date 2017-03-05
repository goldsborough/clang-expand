// Library includes
#include "clang-expand/search.hpp"
#include "clang-expand/common/state.hpp"
#include "clang-expand/definition-search/tool-factory.hpp"
#include "clang-expand/symbol-search/tool-factory.hpp"

// Clang includes
#include "clang/Tooling/Tooling.h"

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

  if (auto* definition = std::get_if<ClangExpand::DefinitionState>(&*_state)) {
    llvm::outs() << definition->code << '\n';
    return EXIT_SUCCESS;
  }


  if (auto error = _definitionSearch(compilationDatabase, sources); error) {
    return error;
  }

  llvm::outs() << std::get<ClangExpand::DefinitionState>(*_state).code << '\n';

  return EXIT_SUCCESS;
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
      _location.filename, std::get<ClangExpand::DeclarationState>(*_state),
      [this](auto&& result) {
        _state = std::move(result);
    }));
  // clang-format on
}

}  // namespace ClangExpand
