#ifndef CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP

// Project includes
#include "clang-expand/common/canonical-location.hpp"

// Clang includes
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/PPCallbacks.h>

// LLVM includes
#include <llvm/ADT/StringMap.h>

// Standard includes
#include <functional>
#include <iosfwd>

namespace clang {
class LangOptions;
class CompilerInstance;
class MacroArgs;
class MacroDefinition;
class MacroInfo;
class Preprocessor;
class SourceManager;
class Token;
}  // namespace clang

namespace llvm {
template <unsigned int InternalLen>
class SmallString;
}

namespace ClangExpand {
struct DefinitionData;
}  // namespace ClangExpand

namespace ClangExpand::SymbolSearch {


struct MacroSearch : public clang::PPCallbacks {
 public:
  using MatchCallback = std::function<void(DefinitionData&&)>;

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
  const CanonicalLocation _callLocation;
  const MatchCallback _callback;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
