// Project includes
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/structures.hpp"

// Clang includes
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/MacroInfo.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return Structures::CanonicalLocation(first, sourceManager) ==
         Structures::CanonicalLocation(second, sourceManager);
}

llvm::StringRef getSourceText(const clang::SourceRange& range,
                              const clang::SourceManager& sourceManager,
                              const clang::LangOptions& languageOptions) {
  const auto charRange =
      clang::Lexer::getAsCharRange(range, sourceManager, languageOptions);

  bool error;
  auto text = clang::Lexer::getSourceText(charRange,
                                          sourceManager,
                                          languageOptions,
                                          &error);
  assert(!error && "Error getting source text");

  return text;
}
}  // namespace ClangExpand::Routines
