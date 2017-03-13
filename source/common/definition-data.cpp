// Project includes
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/canonical-location.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Casting.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdlib>
#include <string>
#include <system_error>
#include <type_traits>

namespace ClangExpand {
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

std::string getRewrittenText(clang::Stmt* body,
                             const Query& query,
                             const clang::SourceLocation& afterBrace,
                             clang::ASTContext& context,
                             clang::Rewriter& rewriter) {
  const auto& map = query.declaration->parameterMap;

  DefinitionRewriter(rewriter, map, query.call, context).TraverseStmt(body);

  const auto beforeBrace = body->getLocEnd().getLocWithOffset(-1);
  const clang::SourceRange range(afterBrace, beforeBrace);

  return withoutIndentation(rewriter.getRewrittenText(range));
}
}  // namespace

DefinitionData DefinitionData::Collect(const clang::FunctionDecl& function,
                                       clang::ASTContext& context,
                                       const Query& query) {
  const auto& sourceManager = context.getSourceManager();
  Location location(function.getLocation(), sourceManager);

  assert(function.hasBody() &&
         "Function should have a body to collect definition");
  auto* body = llvm::cast<clang::CompoundStmt>(function.getBody());

  if (body->body_empty()) return {location, "", ""};

  clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());

  std::string original;
  if (query.options.wantsDefinition) {
    const clang::SourceRange entireFunction(function.getSourceRange());
    original = rewriter.getRewrittenText(entireFunction);
  }

  const auto afterBrace = body->getLocStart().getLocWithOffset(+1);
  if (query.call && query.call->requiresDeclaration()) {
    insertDeclaration(*query.call->assignee, afterBrace, rewriter);
  }

  std::string rewritten;
  if (query.options.wantsRewritten) {
    rewritten = getRewrittenText(body, query, afterBrace, context, rewriter);
  }

  return {std::move(location), std::move(original), std::move(rewritten)};
}
}  // namespace ClangExpand
