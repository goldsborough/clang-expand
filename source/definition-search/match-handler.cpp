// Project includes
#include "clang-expand/definition-search/match-handler.hpp"
#include "clang-expand/common/context-data.hpp"
#include "clang-expand/common/declaration-data.hpp"
#include "clang-expand/common/definition-data.hpp"
#include "clang-expand/common/query.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/AST/Decl.h>
#include <clang/AST/DeclBase.h>
#include <clang/AST/Type.h>
#include <clang/ASTMatchers/ASTMatchers.h>

// LLVM includes
#include <llvm/ADT/Optional.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/Casting.h>

// Standard includes
#include <cassert>
#include <string>
#include <utility>

namespace ClangExpand {
namespace DefinitionSearch {
namespace {
/// Compares a kind of `Context` with an expected `ContextData`. Their kind
/// (namespace, class etc.) and name must match.
template <typename Context>
bool contextMatches(const Context& context,
                    const ContextData& expectedContext) {
  if (context.getDeclKind() != expectedContext.kind) return false;
  if (context.getName() != expectedContext.name) return false;
  return true;
}
}  // namespace

MatchHandler::MatchHandler(Query& query) : _query(query) {
}

void MatchHandler::run(const MatchResult& result) {
  const auto* function = result.Nodes.getNodeAs<clang::FunctionDecl>("fn");
  assert(function != nullptr && "Got null function node in match handler");

  const auto& parameterTypes = _query.declaration->parameterTypes;

  if (function->getNumParams() != parameterTypes.size()) return;

  if (!_matchParameters(*result.Context, *function)) return;
  if (!_matchContexts(*function)) return;

  auto definition = DefinitionData::Collect(*function, *result.Context, _query);
  _query.definition = std::move(definition);
}

bool MatchHandler::_matchParameters(const clang::ASTContext& context,
                                    const clang::FunctionDecl& function) const
    noexcept {
  const auto& policy = context.getPrintingPolicy();

  auto expectedType = _query.declaration->parameterTypes.begin();
  for (const auto* parameter : function.parameters()) {
    const auto type = parameter->getOriginalType().getCanonicalType();
    if (*expectedType != type.getAsString(policy)) return false;
    ++expectedType;
  }

  return true;
}

bool MatchHandler::_matchContexts(const clang::FunctionDecl& function) const
    noexcept {
  auto expectedContext = _query.declaration->contexts.begin();

  const auto* context = function.getPrimaryContext()->getParent();
  for (; context; context = context->getParent()) {
    if (auto* ns = llvm::dyn_cast<clang::NamespaceDecl>(context)) {
      if (!contextMatches(*ns, *expectedContext)) return false;
      ++expectedContext;
    } else if (auto* record = llvm::dyn_cast<clang::RecordDecl>(context)) {
      if (!contextMatches(*record, *expectedContext)) return false;
      ++expectedContext;
    }
  }

  return true;
}

}  // namespace DefinitionSearch
}  // namespace ClangExpand
