// Project includes
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Lex/Lexer.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <string>
#include <system_error>
#include <type_traits>

namespace ClangExpand::Routines {
namespace {
void insertDeclaration(const AssigneeData& assignee,
                       const clang::SourceLocation& location,
                       clang::Rewriter& rewriter) {
  const auto text =
      (llvm::Twine(assignee.type->name) + " " + assignee.name + ";\n").str();
  const auto error = rewriter.InsertTextAfter(location, text);
  assert(!error && "Error inserting declaration at start of body");
}

std::string withoutIndentation(std::string text) {
  auto start = text.begin();
  int offset = -1;
  while (true) {
    // Find the first character after the newline that is not space
    auto end = std::find_if(start, text.end(), [](char character) {
      return !::isspace(character);
    });

    if (start != end) {
      if (start == text.begin()) {
        end = text.erase(start, end);
      } else {
        // Skip the newline, unless this is extra whitespace going until the end
        // of the string. Then we want to get rid of it (like rstrip-ing).
        if (end != text.end()) ++start;

        // If this is the first time we find whitespaace,
        // take this as the reference offset.
        if (offset == -1) offset = end - start;

        // Now erase only as much whitespace as we encountered the first time
        end = text.erase(start, std::min(start + offset, end));
      }
    }

    // Find the next newline
    start = std::find(end, text.end(), '\n');
    if (start == text.end()) break;
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

std::string getSourceText(const clang::SourceRange& range,
                          clang::SourceManager& sourceManager,
                          const clang::LangOptions& languageOptions) {
  clang::Rewriter rewriter(sourceManager, languageOptions);
  return rewriter.getRewrittenText(range);
}

std::string
getSourceText(const clang::SourceRange& range, clang::ASTContext& context) {
  return getSourceText(range,
                       context.getSourceManager(),
                       context.getLangOpts());
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
  DefinitionRewriter(rewriter, parameterMap, call, context).TraverseStmt(body);

  const auto afterBrace = body->getLocStart().getLocWithOffset(+1);
  const auto beforeBrace = body->getLocEnd().getLocWithOffset(-1);
  const clang::SourceRange range(afterBrace, beforeBrace);

  if (call && call->requiresDeclaration()) {
    insertDeclaration(*call->assignee, afterBrace, rewriter);
  }

  const auto text = withoutIndentation(rewriter.getRewrittenText(range));
  return {std::move(location), text};
}

std::string makeAbsolute(const std::string& filename) {
  llvm::SmallString<256> absolutePath(filename);
  const auto failure = llvm::sys::path::remove_dots(absolutePath, true);
  assert(!failure && "Error cleaning path before making it absolute");
  const auto error = llvm::sys::fs::make_absolute(absolutePath);
  assert(!error && "Error generating absolute path");
  return absolutePath.str();
}

void error(const char* message) {
  llvm::errs() << message << '\n';
  std::exit(EXIT_FAILURE);
}

void error(llvm::Twine&& twine) {
  llvm::errs() << twine.str() << '\n';
  std::exit(EXIT_FAILURE);
}

}  // namespace ClangExpand::Routines
