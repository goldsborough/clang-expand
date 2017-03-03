// Project includes
#include "clang-expand/symbol-search/preprocessor-hooks.hpp"

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Token.h"

// LLVM includes
#include "llvm/Support/raw_ostream.h"

// System includes
#include <cassert>

namespace ClangExpand::SymbolSearch {

PreprocessorHooks::PreprocessorHooks(const clang::SourceManager& sourceManager,
                                     const clang::SourceLocation& location,
                                     const MatchCallback& callback)
: _sourceManager(sourceManager)
, _callFile(
      sourceManager.getFileEntryForID(sourceManager.getFileID(location)))
, _callOffset(sourceManager.getFileOffset(location))
, _callback(callback) {
  assert(callback != nullptr);
}

void PreprocessorHooks::MacroExpands(const clang::Token&,
                                     const clang::MacroDefinition& macro,
                                     clang::SourceRange range,
                                     const clang::MacroArgs*) {
  const auto decomposedLocation =
      _sourceManager.getDecomposedLoc(range.getBegin());
  const auto fileEntry =
      _sourceManager.getFileEntryForID(decomposedLocation.first);

  if (fileEntry != _callFile) return;
  if (decomposedLocation.second != _callOffset) return;

  _callback(*macro.getMacroInfo());
}

}  // namespace ClangExpand::SymbolSearch
