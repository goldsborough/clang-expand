#ifndef CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace clang {
class SourceLocation;
}

namespace ClangExpand {
struct Query;
}

namespace ClangExpand::SymbolSearch {

/// \ingroup SymbolSearch
///
/// Handles candidate functions in the source.
///
/// This class does the heaviest lifting of any components inside clang-expand
/// as it has to handle all aspects of:
/// 1. Checking for a function call expression that matches the ASTMatchers
/// expression from the `SymbolSearch::Consumer` (i.e. with the correct name) if
/// that call location is at the right place (under the user's cursor).
/// 2. Verifying if it is safe to expand the function call wherever it is in the
/// source. For example, it is not safe to expand a function call inside another
/// function call.
/// 3. Collecting `CallData`, including the source extent of the call and
/// assignee of any return value.
/// 4. Collecting `DeclarationData`, i.e. as much information as possible to
/// serialize our knowledge for later use in the definition search phase.
/// 5. If the declaration is in fact also a definition, collecting
/// `DefinitionData`.
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  /// Constructor.
  explicit MatchHandler(const clang::SourceLocation& targetLocation,
                        Query& query);

  /// Runs the `MatchHandler` for a matching expression.
  void run(const MatchResult& result) override;

 private:
  /// The target location of the function call.
  const clang::SourceLocation& _targetLocation;

  /// The ongoing `Query` object.
  Query& _query;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_MATCH_HANDLER_HPP
