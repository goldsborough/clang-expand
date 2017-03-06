#ifndef CLANG_EXPAND_COMMON_PARAMETER_REWRITER_HPP
#define CLANG_EXPAND_COMMON_PARAMETER_REWRITER_HPP

// Project includes
#include "clang-expand/common/data.hpp"

// Clang includes
#include <clang/AST/RecursiveASTVisitor.h>

namespace clang {
class Rewriter;
class Stmt;
}

namespace ClangExpand {
class ParameterRewriter : public clang::RecursiveASTVisitor<ParameterRewriter> {
 public:
  explicit ParameterRewriter(const ParameterMap& parameterMap,
                       clang::Rewriter& rewriter);

  bool VisitStmt(clang::Stmt* statement);

 private:
  const ParameterMap& _parameterMap;
  clang::Rewriter& _rewriter;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_PARAMETER_REWRITER_HPP
