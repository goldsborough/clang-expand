#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

namespace clang {
class SourceLocation;
class SourceManager;
}

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager);
}  // namespace ClangExpand::Routines


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
