#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

// Standard includes
#include <iosfwd>

namespace clang {
class SourceLocation;
class SourceManager;
class LangOptions;
class SourceRange;
class ASTContext;
}

namespace llvm {
class Twine;
}

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager);

std::string getSourceText(const clang::SourceRange& range,
                          clang::SourceManager& sourceManager,
                          const clang::LangOptions& languageOptions);

std::string
getSourceText(const clang::SourceRange& range, clang::ASTContext& context);

std::string makeAbsolute(const std::string& filename);

[[noreturn]] void error(const char* message);
[[noreturn]] void error(llvm::Twine&& twine);

}  // namespace ClangExpand::Routines


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
