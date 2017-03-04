#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

namespace clang {
class SourceLocation;
class SourceManager;
class LangOptions;
class SourceRange;
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

}  // namespace ClangExpand::Routines


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
