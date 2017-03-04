// Library includes
#include "clang-expand/symbol-search/match-handler.hpp"
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
auto fileEntryAndOffset(const clang::SourceLocation& location,
                        const clang::SourceManager& sourceManager) {
  const auto decomposed = sourceManager.getDecomposedLoc(location);
  const auto* fileEntry = sourceManager.getFileEntryForID(decomposed.first);
  return std::make_pair(fileEntry, decomposed.second);
}

bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return fileEntryAndOffset(first, sourceManager) ==
         fileEntryAndOffset(second, sourceManager);
}

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

const char*
getBufferPointerAtLocation(const clang::SourceLocation& location,
                           const clang::SourceManager& sourceManager) {
  bool error;
  const char* data = sourceManager.getCharacterData(location, &error);
  assert(!error && "Error loading character data");
  assert(data != nullptr);

  return data;
}

auto getBufferForRange(const clang::SourceRange& range,
                       const clang::SourceManager& sourceManager) {
  const auto* start =
      getBufferPointerAtLocation(range.getBegin(), sourceManager);
  const auto* end = getBufferPointerAtLocation(range.getEnd(), sourceManager);
  return llvm::StringRef(start, end - start + 1);
}

ArgumentMap mapCallParameters(const clang::CallExpr& call,
                              const clang::FunctionDecl& function,
                              const clang::ASTContext& context) {
  ArgumentMap expressions;
  const auto& sourceManager = context.getSourceManager();

  auto parameter = function.param_begin();
  for (const auto* argument : call.arguments()) {
    // We only want to map argument that were actually passed in the call...
    if (llvm::isa<clang::CXXDefaultArgExpr>(argument)) continue;

    const auto originalName = (*parameter)->getName();
    const auto range = argument->getSourceRange();
    const auto callName = getBufferForRange(range, sourceManager);
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

auto collectDefinitionState(const clang::FunctionDecl& function,
                            clang::ASTContext& context,
                            const ArgumentMap& argumentMap) {
  ClangExpand::DefinitionState definition;

  const auto location = context.getFullLoc(function.getLocation());

  definition.filename = context.getSourceManager().getFilename(location);
  definition.line = location.getSpellingLineNumber();
  definition.column = location.getSpellingColumnNumber();

  assert(function.hasBody());
  auto* body = function.getBody();

  clang::Rewriter rewriter(context.getSourceManager(), context.getLangOpts());
  UsageFinder(argumentMap, rewriter).TraverseStmt(body);

  definition.source = rewriter.getRewrittenText(body->getSourceRange());

  return definition;
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
  if (!locationsAreEqual(callLocation, _targetLocation, sourceManager)) {
    return;
  }

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
