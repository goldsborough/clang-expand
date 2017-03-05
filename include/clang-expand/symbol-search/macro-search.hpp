#ifndef CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP

// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include "clang/Basic/LangOptions.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Lex/PPCallbacks.h"
#include "clang/Lex/Preprocessor.h"

// LLVM includes
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"

// Standard includes
#include <functional>
#include <string>

namespace clang {
class CompilerInstance;
class FileEntry;
class MacroArgs;
class MacroDefinition;
class MacroInfo;
class Preprocessor;
class SourceManager;
class SourceRange;
class Token;
}  // namespace clang

namespace ClangExpand {
struct DefinitionState;
}  // namespace ClangExpand

namespace ClangExpand::SymbolSearch {


struct MacroSearch : public clang::PPCallbacks {
 public:
  using MatchCallback = std::function<void(DefinitionState&&)>;

  MacroSearch(clang::CompilerInstance& compiler,
                    const clang::SourceLocation& location,
                    const MatchCallback& callback);

  void MacroExpands(const clang::Token& macroNameToken,
                    const clang::MacroDefinition& macroDefinition,
                    clang::SourceRange range,
                    const clang::MacroArgs* macroArgs) override;

 private:
  using ParameterMapping = llvm::StringMap<llvm::SmallString<32>>;

  std::string
  _rewriteMacro(const clang::MacroInfo& info, const ParameterMapping& mapping);

  ParameterMapping _createParameterMapping(const clang::MacroInfo& info,
                                           const clang::MacroArgs& arguments);

  std::string _getSpelling(const clang::Token& token) const;

  clang::SourceManager& _sourceManager;
  const clang::LangOptions& _languageOptions;
  clang::Preprocessor& _preprocessor;
  const Structures::CanonicalLocation _callLocation;
  const MatchCallback _callback;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
