#ifndef CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP
#define CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"

// Clang includes
#include <clang/AST/Expr.h>
#include <clang/AST/RecursiveASTVisitor.h>

// LLVM includes
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/StringMap.h>

// Standard includes
#include <iosfwd>
#include <optional>

namespace clang {
class ASTContext;
class Rewriter;
class Stmt;
}

namespace ClangExpand {
class DefinitionRewriter
    : public clang::RecursiveASTVisitor<DefinitionRewriter> {
 public:
  using OptionalCall = std::optional<CallData>;
  using ParameterMap = llvm::StringMap<std::string>;

  explicit DefinitionRewriter(clang::Rewriter& rewriter,
                              const ParameterMap& parameterMap,
                              const OptionalCall& call,
                              clang::ASTContext& context);

  bool VisitStmt(clang::Stmt* statement);

 private:
  void _rewriteReturn(const clang::ReturnStmt& returnStatement,
                      const CallData& call);

  void _rewriteMemberExpression(const clang::MemberExpr& member);

  clang::Rewriter& _rewriter;
  const ParameterMap& _parameterMap;
  const OptionalCall& _call;
  clang::ASTContext& _context;
  llvm::SmallPtrSet<const clang::MemberExpr*, 16> _rewrittenMembers;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DEFINITION_REWRITER_HPP
