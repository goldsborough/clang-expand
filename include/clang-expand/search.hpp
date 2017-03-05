#ifndef CLANG_EXPAND_SEARCH_HPP
#define CLANG_EXPAND_SEARCH_HPP

// Project includes
#include "clang-expand/common/state.hpp"

// Standard includes
#include <optional>
#include <string>
#include <vector>

namespace clang {
namespace tooling {
class CompilationDatabase;
}
}

namespace ClangExpand {
class Search {
 public:
  using CompilationDatabase = clang::tooling::CompilationDatabase;
  using SourceVector = std::vector<std::string>;

  Search(const std::string& file, unsigned line, unsigned column);

  int run(CompilationDatabase& compilationDatabase,
          const SourceVector& sources);

 private:
  int _symbolSearch(CompilationDatabase& compilationDatabase);
  int _definitionSearch(CompilationDatabase& compilationDatabase,
                        const SourceVector& sources);

  Structures::EasyLocation _location;
  std::optional<ClangExpand::State> _state;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SEARCH_HPP
