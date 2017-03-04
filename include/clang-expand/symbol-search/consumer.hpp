#ifndef CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP

// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/AST/ASTConsumer.h"
#include "clang/Basic/SourceLocation.h"

// Standard includes
#include <functional>
#include <string>

namespace clang {
class ASTContext;
class MacroInfo;
class FunctionDecl;
class Token;
}

namespace ClangExpand::SymbolSearch {

class Consumer : public clang::ASTConsumer {
 public:
  using LazyBoolean = std::function<bool()>;

  Consumer(const clang::SourceLocation& invocationLocation,
           const std::string& invocationSpelling,
           const LazyBoolean& alreadyFoundMacro,
           const StateCallback& stateCallback);

  void HandleTranslationUnit(clang::ASTContext& Context) override;

 private:
  const std::string _callSpelling;
  const LazyBoolean _alreadyFoundMacro;
  MatchHandler _matchHandler;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_CONSUMER_HPP
