// Project includes
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/location.hpp"
#include "clang-expand/common/query.hpp"

// Third party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/Stmt.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/options.hpp"

// LLVM includes
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <regex>
#include <string>
#include <type_traits>

namespace ClangExpand {
namespace {
/// Removes all excess whitespace around the string, and from the start of each
/// line. This is necessary so that the body of the function can be returned
/// without any extra padding on the left, as it would normally have at least
/// one level of indenting if simply cut out of a real function.
///
/// For example, given this function that we want to rewrite:
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
  // clang-format off
  static const std::regex whitespacePattern(
    R"(^\s*\n(\s+)\S|(\s+))", std::regex::ECMAScript | std::regex::optimize);
  // clang-format on

  std::smatch match;
  if (!std::regex_search(text, match, whitespacePattern)) {
    return text;
  }

  assert(match[1].matched || match[2].matched);
  const std::string excess = match[1].matched ? match.str(1) : match.str(2);

  // C++ regex doesn't have a multiline option (or at least clang doesn't have
  // one yet, should be in C++17), so we need to hack.
  const std::regex excessPattern("\\n" + excess, std::regex::optimize);

  const auto trimmed = llvm::StringRef(text).trim().str();
  return std::regex_replace(trimmed, excessPattern, "\n");
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

  bool shouldDeclare = false;
  if (query.call->assignee) {
    shouldDeclare = definitionRewriter.rewriteReturnsToAssignments(*body);
  }

  const auto afterBrace = body->getLocStart().getLocWithOffset(+1);
  const auto beforeBrace = body->getLocEnd().getLocWithOffset(-1);
  const clang::SourceRange range(afterBrace, beforeBrace);

  auto text = withoutIndentation(rewriter.getRewrittenText(range));

  if (shouldDeclare) {
    const std::string declaration = query.call->assignee->toDeclaration();
    return (declaration + llvm::Twine("\n") + text).str();
  }

  return text;
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

nlohmann::json DefinitionData::toJson() const {
  // clang-format off
  nlohmann::json json = {
    {"location", location.toJson()},
    {"macro", isMacro}
  };
  // clang-format on

  if (!original.empty()) {
    json["text"] = original;
  }

  if (!rewritten.empty()) {
    json["rewritten"] = rewritten;
  }

  return json;
}

}  // namespace ClangExpand
