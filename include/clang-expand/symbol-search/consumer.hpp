#ifndef CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP

// Project includes
#include "clang-expand/symbol-search/match-handler.hpp"

// Clang includes
#include <clang/AST/ASTConsumer.h>

// Standard includes
#include <functional>
#include <iosfwd>
#include <string>

namespace clang {
class ASTContext;
class SourceLocation;
}

namespace ClangExpand {
class Query;
}

namespace ClangExpand::SymbolSearch {

class Consumer : public clang::ASTConsumer {
 public:
  using LazyBoolean = std::function<bool()>;

  Consumer(const clang::SourceLocation& invocationLocation,
           const std::string& invocationSpelling,
           const LazyBoolean& alreadyFoundMacro,
           Query* query);

  void HandleTranslationUnit(clang::ASTContext& context) override;

 private:
  const std::string _callSpelling;
  const LazyBoolean _alreadyFoundMacro;
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
