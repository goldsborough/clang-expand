#ifndef CLANG_EXPAND_OPTIONS_HPP
#define CLANG_EXPAND_OPTIONS_HPP

namespace ClangExpand {
struct Options {
  bool wantsCall;
  bool wantsDeclaration;
  bool wantsDefinition;
  bool wantsRewritten;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_OPTIONS_HPP
