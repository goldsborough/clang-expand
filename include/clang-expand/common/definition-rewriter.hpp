#ifndef CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/data.hpp"

// Clang includes
#include <clang/AST/RecursiveASTVisitor.h>

namespace clang {
class Rewriter;
class Stmt;
}

namespace ClangExpand {
class DefinitionRewriter
    : public clang::RecursiveASTVisitor<DefinitionRewriter> {
 public:
  using OptionalCall = std::optional<CallData>;

  explicit DefinitionRewriter(clang::Rewriter& rewriter,
                              const ParameterMap& parameterMap,
                              const OptionalCall& call);

  bool VisitStmt(clang::Stmt* statement);

 private:
  void _rewriteReturn(const clang::ReturnStmt& returnStatement,
                      const CallData& call);

  clang::Rewriter& _rewriter;
  const ParameterMap& _parameterMap;
  const OptionalCall& _call;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP
