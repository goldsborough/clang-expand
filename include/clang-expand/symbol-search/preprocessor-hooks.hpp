#ifndef CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/PPCallbacks.h"

// Standard includes
#include <functional>
#include <utility>

namespace clang {
class Token;
class MacroDefinition;
class SourceRange;
class MacroArgs;
class MacroInfo;
class SourceManager;
class FileEntry;
}

namespace ClangExpand::SymbolSearch {


struct PreprocessorHooks : public clang::PPCallbacks {
 public:
  using MatchCallback = std::function<void(const clang::MacroInfo&)>;

  PreprocessorHooks(const clang::SourceManager& sourceManager,
                    const clang::SourceLocation& location,
                    const MatchCallback& callback);

  void MacroExpands(const clang::Token& macroNameToken,
                    const clang::MacroDefinition& macroDefinition,
                    clang::SourceRange range,
                    const clang::MacroArgs* macroArgs) override;

 private:
  const clang::SourceManager& _sourceManager;
  const clang::FileEntry* const _callFile;
  const unsigned  _callOffset;
  const MatchCallback _callback;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
