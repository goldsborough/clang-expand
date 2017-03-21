// Project includes
#include "clang-expand/common/definition-rewriter.hpp"
#include "clang-expand/common/assignee-data.hpp"
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/routines.hpp"

// Clang includes
#include <clang/AST/Decl.h>
#include <clang/AST/DeclTemplate.h>
#include <clang/AST/Expr.h>
#include <clang/AST/ExprCXX.h>
#include <clang/AST/Stmt.h>
#include <clang/AST/Type.h>
#include <clang/AST/TypeLoc.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/APInt.h>
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallPtrSet.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <string>


namespace ClangExpand {
namespace {

/// Dies with an error message warning that the function could not be expanded
/// because the assigned type is not default constructible (like `int&`).
[[noreturn]] void dieBecauseNotDefaultConstructible() {
  Routines::error(
      "Could not expand function because "
      "assignee is not default-constructible");
}

/// Tries to get the parent of a node as the given type `T`, or else errors and
/// dies.
template <typename T, typename Node>
const T* tryToGetParentOrDie(clang::ASTContext& context, const Node& node) {
  if (const auto* parent = context.getParents(node).begin()) {
    if (const auto* asType = parent->template get<T>()) {
      return asType;
    }
  }

  dieBecauseNotDefaultConstructible();
}

/// Ensures that a `ReturnStmt` would allow default construction of a variable.
/// This is the case if this is a top-level `return`, i.e. whose parent is the
/// `CompoundStmt` of a function.
void ensureReturnAllowsDefaultConstruction(
    clang::ASTContext& context, const clang::ReturnStmt& returnStatement) {
  const auto* compound =
      tryToGetParentOrDie<clang::CompoundStmt>(context, returnStatement);
  tryToGetParentOrDie<clang::FunctionDecl>(context, *compound);
}
}  // namespace

DefinitionRewriter::DefinitionRewriter(clang::Rewriter& rewriter,
                                       const ParameterMap& parameterMap,
                                       const CallData& call,
                                       clang::ASTContext& context)
: _rewriter(rewriter)
, _parameterMap(parameterMap)
, _call(call)
, _context(context) {
}

bool DefinitionRewriter::VisitStmt(clang::Stmt* statement) {
  const auto* nonType =
      llvm::dyn_cast<clang::SubstNonTypeTemplateParmExpr>(statement);
  if (nonType) {
    _rewriteNonTypeTemplateParameterExpression(*nonType);
  }

  if (llvm::isa<clang::ReturnStmt>(statement)) {
    if (auto* rtn = llvm::dyn_cast<clang::ReturnStmt>(statement)) {
      _recordReturn(*rtn, _call);
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
    (void)error;
    assert(!error && "Error replacing text in definition");
  }

  return true;
}

bool DefinitionRewriter::VisitTypeLoc(clang::TypeLoc typeLocation) {
  const auto* templateType =
      llvm::dyn_cast<clang::SubstTemplateTypeParmType>(typeLocation.getType());
  if (!templateType) return true;

  const auto original =
      templateType->getReplacedParameter()->desugar().getAsString();
  const auto start = typeLocation.getLocStart();
  const auto end =
      typeLocation.getLocStart().getLocWithOffset(original.length() - 1);

  const auto replacement = templateType->getReplacementType().getAsString();
  _rewriter.ReplaceText({start, end}, replacement);

  return true;
}

bool DefinitionRewriter::rewriteReturnsToAssignments(const clang::Stmt& body) {
  assert(_call.assignee.hasValue() &&
         "Cannot rewrite returns to assignments without an assignee");
  assert(!_returnLocations.empty() &&
         "Assigning to a function call that doesn't return?");

  if (_returnLocations.size() == 1) {
    _rewriteReturn(_returnLocations.front(), _call.assignee->toAssignment());
    return false;
  }

  assert(_call.assignee->isDefaultConstructible());

  for (const auto& location : _returnLocations) {
    _rewriteReturn(location, _call.assignee->toAssignment(/*withType=*/false));
  }

  return _call.requiresDeclaration();
}

void DefinitionRewriter::_recordReturn(const clang::ReturnStmt& returnStatement,
                                       const CallData& call) {
  if (!call.assignee.hasValue()) return;
  if (!call.assignee->isDefaultConstructible()) {
    // If we already found a return statement on the top level of the function,
    // then die. This is a super-duper edge case when the code has two return
    // statements on the top function level (making everything underneath the
    // first return dead code).
    if (_returnLocations.empty()) {
      ensureReturnAllowsDefaultConstruction(_context, returnStatement);
    } else {
      dieBecauseNotDefaultConstructible();
    }
  }

  auto location = returnStatement.getSourceRange().getBegin();
  _returnLocations.emplace_back(location);  // trivially-copyable
}

void DefinitionRewriter::_rewriteReturn(const clang::SourceLocation& begin,
                                        const std::string& replacement) {
  static constexpr auto lengthOfTheWordReturn = 6;

  const auto end = begin.getLocWithOffset(lengthOfTheWordReturn);
  const bool error = _rewriter.ReplaceText({begin, end}, replacement);
  assert(!error && "Error replacing return statement in definition");
  (void)error;
}

void DefinitionRewriter::_rewriteMemberExpression(
    const clang::MemberExpr& member) {
  // If the base is empty, this means (should mean) that this function was an
  // implicit access, e.g. calling `f()` inside the class that declares `f()`.
  // Therefore all member expressions will already be valid and don't need any
  // change anyway (i.e. referencing the field `x` by `x` will be fine).
  if (_call.base.empty()) return;
  if (_rewrittenMembers.count(&member)) return;

  if (member.isImplicitAccess()) {
    _rewriter.InsertText(member.getMemberLoc(), _call.base);
  } else {
    // Gobble up any kind of 'this->' statement or qualifier (e.g. super::x,
    // where 'super' is typedef for the base class, i.e. still an implicit
    // access).
    const auto start = member.getLocStart();
    const auto end = member.getMemberLoc().getLocWithOffset(-1);
    _rewriter.ReplaceText({start, end}, _call.base);
  }

  // I've encountered cases where the exact same member will match twice.

  _rewrittenMembers.insert(&member);
}

void DefinitionRewriter::_rewriteNonTypeTemplateParameterExpression(
    const clang::SubstNonTypeTemplateParmExpr& nonType) {
  const auto* expression =
      nonType.getReplacement()->IgnoreImplicit()->IgnoreCasts();
  const auto* integer = llvm::dyn_cast<clang::IntegerLiteral>(expression);
  if (!integer) return;

  const bool isUnsigned =
      nonType.getParameter()->getType().getTypePtr()->isUnsignedIntegerType();
  const auto replacement =
      integer->getValue().toString(10, /*Signed=*/!isUnsigned);

  const auto range = nonType.getSourceRange();
  const bool error = _rewriter.ReplaceText(range, replacement);
  assert(!error && "Error replacing non-type template parameter expression");
  (void)error;
}

}  // namespace ClangExpand
