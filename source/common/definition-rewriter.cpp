// Project includes
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/data.hpp"
#include "clang-expand/common/routines.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
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
namespace {
template <typename T, typename Node>
const T* getIfParentHasType(clang::ASTContext& context, const Node& node) {
  if (const auto* parent = context.getParents(node).begin()) {
    if (const auto* asType = parent->template get<T>()) {
      return asType;
    }
  }

  Routines::error(
      "Could not expand function because "
      "assignee is not default-constructible");
}

void ensureDefaultConstructible(clang::ASTContext& context,
                                const clang::ReturnStmt& returnStatement) {
  const auto* compound =
      getIfParentHasType<clang::CompoundStmt>(context, returnStatement);
  getIfParentHasType<clang::FunctionDecl>(context, *compound);
}

}  // namespace

DefinitionRewriter::DefinitionRewriter(clang::Rewriter& rewriter,
                                       const ParameterMap& parameterMap,
                                       const OptionalCall& call,
                                       clang::ASTContext& context)
: _rewriter(rewriter)
, _parameterMap(parameterMap)
, _call(call)
, _context(context) {
}

bool DefinitionRewriter::VisitStmt(clang::Stmt* statement) {
  if (_call && llvm::isa<clang::ReturnStmt>(statement)) {
    if (auto* rtn = llvm::dyn_cast<clang::ReturnStmt>(statement)) {
      _rewriteReturn(*rtn, *_call);
      return true;
    }
  }

  const auto* reference = llvm::dyn_cast<clang::DeclRefExpr>(statement);
  if (!reference) return true;

  const auto* declaration =
      llvm::dyn_cast<clang::ParmVarDecl>(reference->getDecl());
  if (!declaration) return true;

  const auto name = declaration->getName();

  auto iterator = _parameterMap.find(name);
  if (iterator != _parameterMap.end()) {
    const auto& argument = iterator->getValue();
    bool error = _rewriter.ReplaceText(reference->getSourceRange(), argument);
    assert(!error && "Error replacing text in definition");
  }

  return true;
}

void DefinitionRewriter::_rewriteReturn(
    const clang::ReturnStmt& returnStatement, const CallData& call) {
  static constexpr auto lengthOfTheWordReturn = 6;

  if (!call.assignee) return;
  if (!call.assignee->isDefaultConstructible()) {
    ensureDefaultConstructible(_context, returnStatement);
  }

  const auto begin = returnStatement.getSourceRange().getBegin();
  const auto end = begin.getLocWithOffset(lengthOfTheWordReturn);
  const auto assignment = (call.assignee->name + " " + call.assignee->op).str();

  bool error = _rewriter.ReplaceText({begin, end}, assignment);
  assert(!error && "Error replacing return statement in definition");
}

}  // namespace ClangExpand
