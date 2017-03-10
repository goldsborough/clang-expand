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
#include <string>

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

  if (const auto* member = llvm::dyn_cast<clang::MemberExpr>(statement)) {
    if (llvm::isa<clang::CXXThisExpr>(member->getBase()->IgnoreImplicit())) {
      _rewriteMemberExpression(*member);
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

  std::string operation;
  if (call.assignee->name.back() == ' ') {
    operation = (call.assignee->name + call.assignee->op).str();
  } else {
    operation = (call.assignee->name + " " + call.assignee->op).str();
  }

  const auto begin = returnStatement.getSourceRange().getBegin();
  const auto end = begin.getLocWithOffset(lengthOfTheWordReturn);

  bool error = _rewriter.ReplaceText({begin, end}, operation);
  assert(!error && "Error replacing return statement in definition");
}

void DefinitionRewriter::_rewriteMemberExpression(
    const clang::MemberExpr& member) {
  assert(_call.has_value() &&
         "Found member expression we'd like to replace, but no call data");
  // If the base is empty, this means (should mean) that this function was an
  // implicit access, e.g. calling `f()` inside the class that declares `f()`.
  // Therefore all member expressions will already be valid and don't need any
  // change anyway (i.e. referencing the field `x` by `x` will be fine).
  if (_call->base.empty()) return;
  if (_rewrittenMembers.count(&member)) return;

  if (member.isImplicitAccess()) {
    _rewriter.InsertText(member.getMemberLoc(), _call->base);
  } else {
    // Gobble up any kind of 'this->' statement or qualifier (e.g. super::x,
    // where 'super' is typedef for the base class, i.e. still an implicit
    // access).
    const auto start = member.getLocStart();
    const auto end = member.getMemberLoc().getLocWithOffset(-1);
    _rewriter.ReplaceText({start, end}, _call->base);
  }

  // I've encountered cases where the exact same member will match twice,
  // for mysterious reasons.
  _rewrittenMembers.insert(&member);
}

}  // namespace ClangExpand
