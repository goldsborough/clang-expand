// Project includes
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/parameter-rewriter.hpp"
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>

// Standard includes
#include <cassert>
#include <string>
#include <type_traits>

namespace ClangExpand::Routines {
namespace {
void insertCall(const VariableData& variable,
                const clang::SourceLocation& location,
                clang::Rewriter& rewriter) {
  const auto text =
      (llvm::Twine(variable.type) + " " + variable.name + ";").str();
  const auto error = rewriter.InsertTextAfter(location, text);
  assert(!error && "Error inserting declaration at start of body");
}

std::string withoutIndentation(std::string text) {
  auto last = text.begin();
  while (true) {
    auto start = std::find(last, text.end(), '\n');
    if (start == text.end()) break;
    // Keep the newline (++start)
    auto end = std::find_if(++start, text.end(), [](char character) {
      return !::isspace(character);
    });
    last = text.erase(start, end);
  }

  // RVO
  return text;
}
}  // namespace

bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return CanonicalLocation(first, sourceManager) ==
         CanonicalLocation(second, sourceManager);
}

llvm::StringRef getSourceText(const clang::SourceRange& range,
                              const clang::SourceManager& sourceManager,
                              const clang::LangOptions& languageOptions) {
  const clang::SourceRange adjustedRange(range.getBegin(),
                                         range.getEnd().getLocWithOffset(+1));
  const auto charRange = clang::Lexer::getAsCharRange(adjustedRange,
                                                      sourceManager,
                                                      languageOptions);
  bool error;
  auto text = clang::Lexer::getSourceText(charRange,
                                          sourceManager,
                                          languageOptions,
                                          &error);
  assert(!error && "Error getting source text");

  return text;
}

DefinitionData collectDefinitionData(const clang::FunctionDecl& function,
                                     clang::ASTContext& context,
                                     const ParameterMap& parameterMap,
                                     const OptionalCall& call) {
  const auto& sourceManager = context.getSourceManager();
  EasyLocation location(function.getLocation(), sourceManager);

  assert(function.hasBody());
  auto* body = function.getBody();

  clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());
  ParameterRewriter(rewriter, parameterMap, call).TraverseStmt(body);

  const auto afterBrace = body->getLocStart().getLocWithOffset(+1);
  const auto beforeBrace = body->getLocEnd().getLocWithOffset(-1);
  const clang::SourceRange range(afterBrace, beforeBrace);

  if (call && call->variable) {
    insertCall(*call->variable, afterBrace, rewriter);
  }

  const auto text = withoutIndentation(rewriter.getRewrittenText(range));
  return {std::move(location), text};
}
}  // namespace ClangExpand::Routines
