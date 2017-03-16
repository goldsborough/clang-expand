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
struct Query;
}  // namespace ClangExpand

namespace ClangExpand::SymbolSearch {

/// Class responsible for inspecting macros during symbol search.
///
/// For a given invocation `f(x)`, we don't know from the raw source text if `f`
/// is a function or a macro. Also, at the point where we have the chance to
/// hook  into the preprocessor (inside the `SymbolSearch::Action`), we don't
/// yet have an AST, so we cannot find this information out. As such, we need to
/// hook into the prepocessing stage and look out for macro invocations. If
/// there is one such invocation whose location matches the cursor, we have
/// determined that the function call is actually a macro expansion and we can
/// process it straight away into a `DefinitionData` object, since macros must
/// always be defined on the spot. Since translation units are preprocessed
/// anyway irrespective of whether or not we need something from this stage,
/// this functioncality incurs very little performance overhead.
struct MacroSearch : public clang::PPCallbacks {
 public:
  /// Constructor.
  MacroSearch(clang::CompilerInstance& compiler,
              const clang::SourceLocation& location,
              Query& query);

  /// Hook for any macro expansion. A macro expansion will either be a
  /// function-macro call like `f(x)`, or simply an object-macro expansion like
  /// `NULL` (which is `(void*)0`).
  void MacroExpands(const clang::Token& macroNameToken,
                    const clang::MacroDefinition& macroDefinition,
                    clang::SourceRange range,
                    const clang::MacroArgs* macroArgs) override;

 private:
  using ParameterMap = llvm::StringMap<llvm::SmallString<32>>;

  /// Rewrites a function-macro contents using the arguments it was invoked
  /// with. This function identifies `#` and `##` stringification and
  /// concatenation operators and deals with them correctly.
  std::string
  _rewriteMacro(const clang::MacroInfo& info, const ParameterMap& mapping);

  /// Creates a mapping from parameter names to argument expressions.
  ParameterMap _createParameterMap(const clang::MacroInfo& info,
                                   const clang::MacroArgs& arguments);

  /// Gets the spelling (string representation) of a token using the
  /// preprocessor.
  std::string _getSpelling(const clang::Token& token) const;  // NOLINT

  /// The current `clang::SourceManager` from the compiler.
  clang::SourceManager& _sourceManager;

  /// The current `clang::LangOptions` from the compiler.
  const clang::LangOptions& _languageOptions;

  /// The `clang::Preprocessor` instance we operate on.
  clang::Preprocessor& _preprocessor;

  /// The canonical location of the (function) call that we are targeting.
  const CanonicalLocation _targetLocation;

  /// The ongoing `Query` object.
  Query& _query;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_PREPROCESSOR_HOOKS_HPP
