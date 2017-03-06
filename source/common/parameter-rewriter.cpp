// Project includes
#include "clang-expand/common/parameter-rewriter.hpp"
#include "clang-expand/common/data.hpp"

// Clang includes
#include <clang/AST/Decl.h>
#include <clang/AST/Expr.h>
#include <clang/AST/Stmt.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>

namespace ClangExpand {

ParameterRewriter::ParameterRewriter(const ParameterMap& parameterMap,
                                     clang::Rewriter& rewriter)
: _parameterMap(parameterMap), _rewriter(rewriter) {
}

bool ParameterRewriter::VisitStmt(clang::Stmt* statement) {
  const auto* use = llvm::dyn_cast<clang::DeclRefExpr>(statement);
  if (!use) return true;

  const auto* decl = llvm::dyn_cast<clang::ParmVarDecl>(use->getDecl());
  if (!decl) return true;

  const auto name = decl->getName();

  auto iterator = _parameterMap.find(name);
  if (iterator != _parameterMap.end()) {
    const auto& argument = iterator->getValue();
    bool error = _rewriter.ReplaceText(use->getSourceRange(), argument);
    assert(!error && "Error replacing text in definition");
  }

  return true;
}

}  // namespace ClangExpand
