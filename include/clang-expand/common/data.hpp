#ifndef CLANG_EXPAND_COMMON_DATA_HPP
#define CLANG_EXPAND_COMMON_DATA_HPP

// Project includes
#include "clang-expand/common/structures.hpp"

// Clang includes
#include <clang/AST/DeclBase.h>

// LLVM includes
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringMap.h>
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <optional>
#include <string>

namespace clang {
class ASTContext;
class FunctionDecl;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand {

struct ContextData {
  ContextData(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_.str()) {
  }

  clang::Decl::Kind kind;
  std::string name;
};

using ContextDataVector = llvm::SmallVector<ContextData, 8>;
using TypeVector = llvm::SmallVector<std::string, 8>;
using ParameterMap = llvm::StringMap<std::string>;

struct VariableData {
  std::string type;
  std::string name;
};

struct CallData {
  CallData(const std::string& type, const std::string& name, Range extent_)
  : variable({type, name}), extent(extent_) {
  }

  explicit CallData(Range extent_) : extent(extent_) {
  }

  std::optional<VariableData> variable;
  Range extent;
};

struct DeclarationData {
  explicit DeclarationData(const llvm::StringRef& name_) : name(name_.str()) {
  }

  std::string name;
  ContextDataVector contexts;
  TypeVector parameterTypes;
  ParameterMap parameterMap;
};

struct DefinitionData {
  EasyLocation location;
  std::string code;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_DATA_HPP
