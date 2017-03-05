#ifndef CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP

// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include "clang/AST/DeclBase.h"

// LLVM includes
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <functional>
#include <string>
#include <variant>

namespace ClangExpand {

struct Context {
  Context(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_) {
  }

  clang::Decl::Kind kind;
  std::string name;
};

using ContextVector = llvm::SmallVector<Context, 8>;
using TypeVector = llvm::SmallVector<std::string, 8>;
using ArgumentMap = llvm::StringMap<llvm::StringRef>;

struct DeclarationState {
  explicit DeclarationState(const llvm::StringRef& name_) : name(name_.str()) {
  }

  std::string name;
  ContextVector contexts;
  TypeVector parameterTypes;
  ArgumentMap argumentMap;
};

struct DefinitionState {
  Structures::EasyLocation location;
  std::string code;
};

using State = std::variant<std::monostate, DeclarationState, DefinitionState>;
using StateCallback = std::function<void(State&&)>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP
