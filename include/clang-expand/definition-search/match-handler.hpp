#ifndef CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
#define CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP

// Clang includes
#include <clang/ASTMatchers/ASTMatchFinder.h>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace ClangExpand {
struct Query;
}

namespace ClangExpand { namespace DefinitionSearch {

/// \ingroup DefinitionSearch
///
/// The match handler of the definition search phase.
///
/// This class is used to match on functions, check that they are the kind we
/// expect from the `DeclarationData` we collected and finally collect
/// `DefinitionData` which contains the definition text of the function we are
/// matching, as well as possibly rewritten (expanded) source text.
class MatchHandler : public clang::ast_matchers::MatchFinder::MatchCallback {
 public:
  using MatchResult = clang::ast_matchers::MatchFinder::MatchResult;

  /// Constructs the `MatchHandler` with the ongoing `Query` object.
  explicit MatchHandler(Query& query);

  /// Runs the `MatchHandler` for a matching function.
  void run(const MatchResult& result) override;

 private:
  /// Compares the parameters of a function with those expected in the query's
  /// `DeclarationData`.
  bool _matchParameters(const clang::ASTContext& context,
                        const clang::FunctionDecl& function) const noexcept;

  /// Compares the contexts of a function with those expected in the query's
  /// `DeclarationData`.
  bool _matchContexts(const clang::FunctionDecl& function) const noexcept;

  /// The ongoing query object.
  Query& _query;
};

}  // namespace DefinitionSearch
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_DEFINITION_SEARCH_MATCH_HANDLER_HPP
