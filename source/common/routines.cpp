// Project includes
#include "clang-expand/common/routines.hpp"
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

// Standard includes
#include <cassert>
#include <type_traits>

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

DefinitionState collectDefinitionState(const clang::FunctionDecl& function,
                                       clang::ASTContext& context,
                                       const ParameterMap& parameterMap) {
  const auto& sourceManager = context.getSourceManager();
  Structures::EasyLocation location(function.getLocation(), sourceManager);

  assert(function.hasBody());
  auto* body = function.getBody();

  clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());
  ParameterRewriter(parameterMap, rewriter).TraverseStmt(body);

  const auto text = rewriter.getRewrittenText(body->getSourceRange());
  return {std::move(location), text};
}
}  // namespace ClangExpand::Routines
