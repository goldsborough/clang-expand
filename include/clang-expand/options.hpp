#ifndef CLANG_EXPAND_OPTIONS_HPP
#define CLANG_EXPAND_OPTIONS_HPP

namespace ClangExpand {
/// Options for a query.
struct Options {
  /// Whether to include information about the function call in the result.
  bool wantsCall;

  /// Whether to include declaration information for the function.
  bool wantsDeclaration;

  /// Whether to include definition information for the function.
  bool wantsDefinition;

  /// Whether to include the rewritten funtcion body information for the
  /// function.
  bool wantsRewritten;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_OPTIONS_HPP
