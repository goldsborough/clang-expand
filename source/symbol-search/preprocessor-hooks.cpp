// Project includes
#include "clang-expand/symbol-search/preprocessor-hooks.hpp"

// Clang includes
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Lexer.h"
#include "clang/Lex/MacroArgs.h"
#include "clang/Lex/MacroInfo.h"
#include "clang/Lex/Token.h"
#include "clang/Lex/TokenLexer.h"

// LLVM includes
#include "llvm/ADT/SmallString.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/raw_ostream.h"

// System includes
#include <cassert>

namespace ClangExpand::SymbolSearch {
namespace {
auto mapCallArguments(const clang::SourceManager& sourceManager,
                      const clang::LangOptions& languageOptions,
                      clang::Preprocessor& preprocessor,
                      const clang::MacroInfo& info,
                      const clang::MacroArgs& arguments) {
  llvm::StringMap<llvm::SmallString<32>> map;
  if (info.getNumArgs() == 0) return map;

  unsigned number = 0;
  for (const auto* parameter : info.args()) {
    const auto* firstToken = arguments.getUnexpArgument(number);
    auto numberOfTokens = arguments.getArgLength(firstToken);
    clang::TokenLexer lexer(firstToken,
                            numberOfTokens,
                            /*DisableExpansion=*/true,
                            false,
                            preprocessor);

    llvm::SmallString<32> wholeArgument;
    while (numberOfTokens-- > 0) {
      clang::Token token;
      [[maybe_unused]] bool ok = lexer.Lex(token);
      assert(ok && "Error lexing token in macro invocation");

      wholeArgument +=
          clang::Lexer::getSpelling(token, sourceManager, languageOptions);
    }

    map[parameter->getName()] = std::move(wholeArgument);
    number += 1;
  }

  return map;
}

const clang::FileEntry*
fileEntryOfLocation(const clang::SourceLocation& location,
                    const clang::SourceManager& sourceManager) {
  const auto id = sourceManager.getFileID(location);
  return sourceManager.getFileEntryForID(id);
}
}  // namespace

PreprocessorHooks::PreprocessorHooks(clang::CompilerInstance& compiler,
                                     const clang::SourceLocation& location,
                                     const MatchCallback& callback)
: _sourceManager(compiler.getSourceManager())
, _languageOptions(compiler.getLangOpts())
, _preprocessor(compiler.getPreprocessor())
, _callFile(fileEntryOfLocation(location, _sourceManager))
, _callOffset(_sourceManager.getFileOffset(location))
, _callback(callback) {
  assert(callback != nullptr);
}

void PreprocessorHooks::MacroExpands(const clang::Token&,
                                     const clang::MacroDefinition& macro,
                                     clang::SourceRange range,
                                     const clang::MacroArgs* arguments) {
  const auto decomposedLocation =
      _sourceManager.getDecomposedLoc(range.getBegin());
  const auto fileEntry =
      _sourceManager.getFileEntryForID(decomposedLocation.first);

  if (fileEntry != _callFile) return;
  if (decomposedLocation.second != _callOffset) return;

  const auto* info = macro.getMacroInfo();
  const auto mapping = mapCallArguments(_sourceManager,
                                  _languageOptions,
                                  _preprocessor,
                                  *info,
                                  *arguments);

  clang::tok::TokenKind lastKind = clang::tok::unknown;
  for (const auto& token : info->tokens()) {
    if (token.getKind() == clang::tok::identifier) {
      const auto identifier =
          clang::Lexer::getSpelling(token, _sourceManager, _languageOptions);
      llvm::outs() << identifier << " -> ";
      if (lastKind == clang::tok::hash) {
        llvm::outs() << '"' << mapping.lookup(identifier) << '"' << '\n';
      } else {
        llvm::outs() << mapping.lookup(identifier) << '\n';
      }
    }
    lastKind = token.getKind();
  }
}

}  // namespace ClangExpand::SymbolSearch
