#ifndef CLANG_EXPAND_SEARCH_HPP
#define CLANG_EXPAND_SEARCH_HPP

// Project includes

#include "clang-expand/common/location.hpp"

// Standard includes
#include <string>
#include <vector>

namespace clang {
namespace tooling {
class CompilationDatabase;
}
}

namespace ClangExpand {
struct Query;
struct Result;
struct Options;

class Search {
 public:
  using CompilationDatabase = clang::tooling::CompilationDatabase;
  using SourceVector = std::vector<std::string>;

  Search(const std::string& file, unsigned line, unsigned column);

  Result run(CompilationDatabase& compilationDatabase,
             const SourceVector& sources,
             const Options& options);

 private:
  void _symbolSearch(CompilationDatabase& compilationDatabase, Query& query);
  void _definitionSearch(CompilationDatabase& compilationDatabase,
                         const SourceVector& sources,
                         Query& query);

  Location _location;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SEARCH_HPP
