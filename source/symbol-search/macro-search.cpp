//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

// Project includes
#include "clang-expand/symbol-search/macro-search.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/range.hpp"
#include "clang-expand/common/routines.hpp"

// Clang includes
#include <clang/Basic/IdentifierTable.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Basic/TokenKinds.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Lexer.h>
#include <clang/Lex/MacroArgs.h>
#include <clang/Lex/MacroInfo.h>
#include <clang/Lex/Token.h>
#include <clang/Lex/TokenLexer.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>

// System includes
#include <cassert>
#include <iterator>
#include <string>
#include <type_traits>


namespace clang {
class LangOptions;
}

namespace ClangExpand {
namespace SymbolSearch {
namespace {

/// Gets the raw source text of a macro definition.
std::string getDefinitionText(const clang::MacroInfo& info,
                              clang::SourceManager& sourceManager,
                              const clang::LangOptions& languageOptions) {
  // Using the rewriter (without actually rewriting) is honestly the only way I
  // found to get at the raw source text in a macro-safe way.
  const auto start = info.tokens_begin()->getLocation();
  const auto end = std::prev(info.tokens_end())->getEndLoc();
  return Routines::getSourceText({start, end}, sourceManager, languageOptions);
}

/// Rewrites a macro argument use inside a macro in case the parameter it maps
/// to was found to be preceded with a `#` stringification operator. It
/// basically quotes it.
void rewriteStringifiedMacroArgument(clang::Rewriter& rewriter,
                                     const clang::Token& token,
                                     const llvm::StringRef& mappedParameter) {
  const clang::SourceRange range(token.getLocation().getLocWithOffset(-1),
                                 token.getEndLoc());
  auto replacement = (llvm::Twine("\"") + mappedParameter + "\"").str();
  rewriter.ReplaceText(range, std::move(replacement));
}

/// Rewrites a macro argument use inside a macro when it is just a simple use
/// and not stringified.
void rewriteSimpleMacroArgument(clang::Rewriter& rewriter,
                                const clang::Token& token,
                                const llvm::StringRef& mappedParameter,
                                unsigned hashCount) {
  const auto offset = -static_cast<int>(hashCount);
  const clang::SourceRange range(token.getLocation().getLocWithOffset(-1),
                                 token.getEndLoc().getLocWithOffset(offset));
  rewriter.ReplaceText(range, mappedParameter);
}
}  // namespace

MacroSearch::MacroSearch(clang::CompilerInstance& compiler,
                         const clang::SourceLocation& location,
                         Query& query)
: _sourceManager(compiler.getSourceManager())
, _languageOptions(compiler.getLangOpts())
, _preprocessor(compiler.getPreprocessor())
, _targetLocation(location, _sourceManager)
, _query(query) {
}

void MacroSearch::MacroExpands(const clang::Token& macroNameToken,
                               const clang::MacroDefinition& macro,
                               clang::SourceRange range,
                               const clang::MacroArgs* arguments) {
  CanonicalLocation canonical(range.getBegin(), _sourceManager);
  if (_targetLocation != canonical) return;

  const auto* info = macro.getMacroInfo();
  auto original = getDefinitionText(*info, _sourceManager, _languageOptions);

  const auto mapping = _createParameterMap(*info, *arguments);
  std::string text = _rewriteMacro(*info, mapping);

  Location location(info->getDefinitionLoc(), _sourceManager);

  if (info->isObjectLike()) {
    // - 1 because the range is inclusive
    const auto length = macroNameToken.getLength() - 1;
    range.setEnd(range.getBegin().getLocWithOffset(length));
  }

  _query.call.emplace(Range{range, _sourceManager});
  _query.definition = DefinitionData{std::move(location),
                                     std::move(original),
                                     std::move(text),
                                     /*isMacro=*/true};
}

std::string MacroSearch::_rewriteMacro(const clang::MacroInfo& info,
                                       const ParameterMap& mapping) {
  clang::Rewriter rewriter(_sourceManager, _languageOptions);

  // Anytime we encounter a hash, we add 1 to this count. Once we are at an
  // identifier, we see how many hashes were right before it. If there are no
  // hashes, we just replace the identifier with the appropriate argument. If
  // there is one hash, we quote the argument. If there are two hashes (the
  // concatenation operator), we do the same thing as when there are no hashes,
  // since the concatenation is implicit for textual replacement. I.e. for
  // `foo_##arg_bar` where `arg` maps to `12` we can just replace this with
  // `foo_12_bar`.
  unsigned hashCount = 0;
  for (const auto& token : info.tokens()) {
    if (token.getKind() == clang::tok::identifier) {
      const auto identifier = _getSpelling(token);

      auto iterator = mapping.find(identifier);
      if (iterator != mapping.end()) {
        const auto mapped = iterator->getValue().str();

        if (hashCount == 1) {
          rewriteStringifiedMacroArgument(rewriter, token, mapped);
        } else {
          rewriteSimpleMacroArgument(rewriter, token, mapped, hashCount);
        }
      }
    }

    if (token.getKind() == clang::tok::hash) {
      hashCount += 1;
    } else {
      hashCount = 0;
    }
  }

  const auto start = info.tokens_begin()->getLocation();
  const auto end = std::prev(info.tokens_end())->getEndLoc();
  return rewriter.getRewrittenText({start, end});
}

MacroSearch::ParameterMap MacroSearch::_createParameterMap(
    const clang::MacroInfo& info, const clang::MacroArgs& arguments) {
  ParameterMap mapping;
  if (info.getNumParams() == 0) return mapping;

  unsigned number = 0;
  for (const auto* parameter : info.params()) {
    const auto* firstToken = arguments.getUnexpArgument(number);
    auto numberOfTokens = arguments.getArgLength(firstToken);
    clang::TokenLexer lexer(firstToken,
                            numberOfTokens,
                            /* DisableMacroExpansion= */ true,
                            /* OwnsTokens= */ false,
                            /* IsReinject= */ false,
                            _preprocessor);

    llvm::SmallString<32> wholeArgument;
    while (numberOfTokens-- > 0) {
      clang::Token token;
      bool ok = lexer.Lex(token);
      (void)ok;
      assert(ok && "Error lexing token in macro invocation");
      wholeArgument += _getSpelling(token);
    }

    mapping[parameter->getName()] = std::move(wholeArgument);
    number += 1;
  }

  return mapping;
}

std::string MacroSearch::_getSpelling(const clang::Token& token) const {
  return clang::Lexer::getSpelling(token, _sourceManager, _languageOptions);
}

}  // namespace SymbolSearch
}  // namespace ClangExpand
