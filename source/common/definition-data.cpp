// Project includes
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/options.hpp"

// LLVM includes
#include <llvm/Support/Casting.h>

// Standard includes
#include <algorithm>
#include <cassert>
#include <cctype>
#include <optional>
#include <string>
#include <type_traits>

namespace ClangExpand {
namespace {
/// Removes all excess whitespace around the string, and from the start of each
/// line. This is necessary so that the body of the function can be returned
/// without any extra padding on the left, as it would normally have at least
/// one level of indenting if simply cut out of a real function.
///
/// This "algoithm" will "normalize" indentation by replacing the outermost
/// amount of indentation from each line. It does this by looking at the first
/// line (that is not pure whitespace) and recording how much whitespace it has.
/// It then removes that amount of whitespace from each subsequent line. For
/// example, given this function that we want to rewrite:
///
/// ```.cpp
/// bool f(int x) {
///   int y = 5;
///   if (x + y > 5) {
///     return true;
///   }
///   return false;
/// }
/// ```
///
/// We may extract the body first as such:
///
/// ```.cpp
///   int y = 5;
///   if (x + y > 5) {
///     return true;
///   }
///   return false;
/// ```
/// and this function then turns it into this normalized snippet:
///
/// ```.cpp
/// int y = 5;
/// if (x + y > 5) {
///   return true;
/// }
/// return false;
/// ```
///
/// Note how only the first level of indentation is removed. Further levels are
/// maintained as expected.
///
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

/// Returns the fully processed rewritten text of a function body.
std::string getRewrittenText(clang::Stmt* body,
                             const Query& query,
                             clang::ASTContext& context,
                             clang::Rewriter& rewriter) {
  const auto& map = query.declaration->parameterMap;

  assert(query.call && "Should have call data when rewriting the definition");

  DefinitionRewriter definitionRewriter(rewriter, map, *query.call, context);
  definitionRewriter.TraverseStmt(body);

  if (query.call->assignee) {
    definitionRewriter.rewriteReturnsToAssignments(*body);
  }

  const auto afterBrace = body->getLocStart().getLocWithOffset(+1);
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

  std::string rewritten;
  if (query.options.wantsRewritten) {
    rewritten = getRewrittenText(body, query, context, rewriter);
  }

  return {std::move(location), std::move(original), std::move(rewritten)};
}
}  // namespace ClangExpand
