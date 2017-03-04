#ifndef CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP

// Clang includes
#include "clang/AST/DeclBase.h"

// LLVM includes
#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <string>
#include <variant>

namespace ClangExpand {

struct Context {
  clang::Decl::Kind kind;
  std::string name;
};

struct DeclarationState {
  explicit DeclarationState(const llvm::StringRef& name_) : name(name_.str()) {
  }

  std::string name;
  llvm::SmallVector<Context, 8> contexts;
  llvm::SmallVector<std::string, 8> parameterTypes;
};

struct DefinitionState {
  std::string filename;
  unsigned line;
  unsigned column;
  std::string source;
};

using State = std::variant<std::monostate, DeclarationState, DefinitionState>;

}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_STATE_HPP
