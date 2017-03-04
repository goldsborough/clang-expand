// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/AST/Decl.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Rewrite/Core/Rewriter.h"

// LLVM includes
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/raw_ostream.h"

// Standard includes
#include <cassert>
#include <functional>
#include <string>

namespace ClangExpand::SymbolSearch {
namespace {
auto collectDeclarationState(const clang::FunctionDecl& function,
                             const clang::ASTContext& astContext) {
  ClangExpand::DeclarationState declaration(function.getName());

  const auto& policy = astContext.getPrintingPolicy();

  // Collect parameter types (their string representations)
  for (const auto* parameter : function.parameters()) {
    const auto type = parameter->getOriginalType().getCanonicalType();
    declaration.parameterTypes.emplace_back(type.getAsString(policy));
  }

  // Collect contexts (their kind, e.g. namespace or class, and name)
  const auto* context = function.getPrimaryContext()->getParent();
  for (; context; context = context->getParent()) {
    const auto kind = context->getDeclKind();
    if (auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      declaration.contexts.emplace_back(kind, ns->getName());
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      declaration.contexts.emplace_back(kind, record->getName());
    }
  }

  return declaration;
}

ArgumentMap mapCallParameters(const clang::CallExpr& call,
                              const clang::FunctionDecl& function,
                              const clang::ASTContext& context) {
  ArgumentMap expressions;
  const auto& sourceManager = context.getSourceManager();
  const auto& languageOptions = context.getLangOpts();

  auto parameter = function.param_begin();
  for (const auto* argument : call.arguments()) {
    // We only want to map argument that were actually passed in the call...
    if (llvm::isa<clang::CXXDefaultArgExpr>(argument)) continue;

    const auto originalName = (*parameter)->getName();
    const auto range = argument->getSourceRange();
    const auto callName =
        Routines::getSourceText(range, sourceManager, languageOptions);
    expressions.insert({originalName, callName});

    ++parameter;
  }

  return expressions;
}

class UsageFinder : public clang::RecursiveASTVisitor<UsageFinder> {
 public:
  explicit UsageFinder(const ArgumentMap& argumentMap,
                       clang::Rewriter& rewriter)
  : _argumentMap(argumentMap), _rewriter(rewriter) {
  }

  bool VisitStmt(clang::Stmt* statement) {
    const auto* use = llvm::dyn_cast<clang::DeclRefExpr>(statement);
    if (!use) return true;

    const auto* decl = llvm::dyn_cast<clang::ParmVarDecl>(use->getDecl());
    if (!decl) return true;

    const auto name = decl->getName();

    auto iterator = _argumentMap.find(name);
    if (iterator != _argumentMap.end()) {
      const auto& argument = iterator->getValue();
      bool error = _rewriter.ReplaceText(use->getSourceRange(), argument);
      assert(!error && "Error replacing text in definition");
    }

    return true;
  }

 private:
  const ArgumentMap& _argumentMap;
  clang::Rewriter& _rewriter;
};

ClangExpand::DefinitionState
collectDefinitionState(const clang::FunctionDecl& function,
                       clang::ASTContext& context,
                       const ArgumentMap& argumentMap) {
  const auto& sourceManager = context.getSourceManager();
  Structures::EasyLocation location(function.getLocation(), sourceManager);

  assert(function.hasBody());
  auto* body = function.getBody();

  clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());
  UsageFinder(argumentMap, rewriter).TraverseStmt(body);

  const auto text = rewriter.getRewrittenText(body->getSourceRange());

  return {std::move(location), text};
}

}  // namespace

MatchHandler::MatchHandler(const clang::SourceLocation& targetLocation,
                           const StateCallback& stateCallback)
: _targetLocation(targetLocation), _stateCallback(stateCallback) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* ref = result.Nodes.getNodeAs<clang::DeclRefExpr>("ref");
  assert(ref != nullptr);

  const auto& sourceManager = *result.SourceManager;
  const auto callLocation = ref->getLocation();
  const auto foundRightCall =
      Routines::locationsAreEqual(callLocation, _targetLocation, sourceManager);
  if (!foundRightCall) return;

  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr);

  auto& context = *result.Context;
  const auto* call = result.Nodes.getNodeAs<clang::CallExpr>("call");
  auto argumentMap = mapCallParameters(*call, *function, context);

  if (function->hasBody()) {
    _stateCallback(collectDefinitionState(*function, context, argumentMap));
  } else {
    auto declaration = collectDeclarationState(*function, context);
    declaration.argumentMap = std::move(argumentMap);
    _stateCallback(std::move(declaration));
  }
}

}  // namespace ClangExpand::SymbolSearch
