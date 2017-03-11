#ifndef CLANG_EXPAND_SEARCH_HPP
#define CLANG_EXPAND_SEARCH_HPP

// Project includes

#include "clang-expand/common/location.hpp"

// Standard includes
#include <string>
#include <variant>
#include <vector>

namespace clang {
namespace tooling {
class CompilationDatabase;
}
}

namespace ClangExpand {
class Query;
struct Result;

class Search {
 public:
  using CompilationDatabase = clang::tooling::CompilationDatabase;
  using SourceVector = std::vector<std::string>;

  Search(const std::string& file,
         unsigned line,
         unsigned column,
         bool shouldRewrite);

  Result
  run(CompilationDatabase& compilationDatabase, const SourceVector& sources);

 private:
  void _symbolSearch(CompilationDatabase& compilationDatabase, Query& query);
  void _definitionSearch(CompilationDatabase& compilationDatabase,
                         const SourceVector& sources,
                         Query& query);

  Location _location;
  bool _shouldRewrite;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SEARCH_HPP
