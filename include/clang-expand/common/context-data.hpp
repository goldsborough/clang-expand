#ifndef CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
#define CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP

// Clang includes
#include <clang/AST/DeclBase.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace ClangExpand {

struct ContextData {
  ContextData(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_.str()) {
  }

  clang::Decl::Kind kind;
  std::string name;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
