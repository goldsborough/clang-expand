// Library includes
#include "clang-expand/definition-search/match-handler.hpp"
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

namespace ClangExpand::DefinitionSearch {
MatchHandler::MatchHandler(const DeclarationState& declaration,
                           const StateCallback& stateCallback)
: _declaration(declaration), _stateCallback(stateCallback) {
}

void MatchHandler::run(const MatchResult& result) {
}

}  // namespace ClangExpand::DefinitionSearch
