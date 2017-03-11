#ifndef CLANG_EXPAND_RESULT_HPP
#define CLANG_EXPAND_RESULT_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"


// Standard includes
#include <iosfwd>

namespace ClangExpand {
class Query;

struct Result {
  explicit Result(Query&& query);
  CallData call;
  DeclarationData declaration;
  DefinitionData definition;
};

llvm::raw_ostream& operator<<(llvm::raw_ostream& stream, const Result& result);

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_RESULT_HPP
