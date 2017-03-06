#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

// Project includes
#include "clang-expand/common/state.hpp"

namespace clang {
class SourceLocation;
class SourceManager;
class LangOptions;
class SourceRange;
class FunctionDecl;
class ASTContext;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager);

llvm::StringRef getSourceText(const clang::SourceRange& range,
                              const clang::SourceManager& sourceManager,
                              const clang::LangOptions& languageOptions);

DefinitionData collectDefinitionData(const clang::FunctionDecl& function,
                                       clang::ASTContext& context,
                                       const ParameterMap& parameterMap);

}  // namespace ClangExpand::Routines


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
