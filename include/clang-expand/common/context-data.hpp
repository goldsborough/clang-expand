#ifndef CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
#define CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP

// Clang includes
#include <clang/AST/DeclBase.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace ClangExpand {

/// Stores information about a context (namespace, class name etc.).
///
/// C++ allows classes inside namespaces to have the same name as the namespace
/// (`namespace X { struct X {}; }`), so we can't just store the name, we also
/// need the `Decl::Kind`.
struct ContextData {
  /// Constructor.
  ContextData(clang::Decl::Kind kind_, const llvm::StringRef& name_)
  : kind(kind_), name(name_.str()) {
  }

  /// The `clang::Decl::Kind` of the context (usually CXXRecord or Namespace).
  clang::Decl::Kind kind;

  /// The name of the context (class, namespace etc.).
  std::string name;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_COMMON_CONTEXT_DATA_HPP
