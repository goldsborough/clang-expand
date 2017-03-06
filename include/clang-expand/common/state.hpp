#ifndef CLANG_EXPAND_COMMON_STATE_HPP
#define CLANG_EXPAND_COMMON_STATE_HPP

// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/AST/DeclBase.h>

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <functional>
#include <string>
#include <variant>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand {

struct ExpectedContext {
  ExpectedContext(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_.str()) {
  }

  clang::Decl::Kind kind;
  std::string name;
};

using ExpectedContextVector = llvm::SmallVector<ExpectedContext, 8>;
using TypeVector = llvm::SmallVector<std::string, 8>;
using ParameterMap = llvm::StringMap<std::string>;

struct CallData {
  std::string declaration;
  Range extent;
};

struct DeclarationData {
  explicit DeclarationData(const llvm::StringRef& name_) : name(name_.str()) {
  }

  std::string name;
  ExpectedContextVector contexts;
  TypeVector parameterTypes;
  ParameterMap parameterMap;
};

struct DefinitionData {
  EasyLocation location;
  std::string code;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_STATE_HPP
