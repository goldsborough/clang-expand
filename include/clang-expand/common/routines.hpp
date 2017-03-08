#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

// Project includes
#include "clang-expand/common/call-data.hpp"
#include "clang-expand/common/data.hpp"

// Standard includes
#include <iosfwd>
#include <optional>

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
class Twine;
}

namespace ClangExpand {
struct DefinitionData;
}

namespace ClangExpand::Routines {
using OptionalCall = std::optional<CallData>;

bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager);

llvm::StringRef getSourceText(const clang::SourceRange& range,
                              const clang::SourceManager& sourceManager,
                              const clang::LangOptions& languageOptions,
                              unsigned offsetAtEnd = +1);

DefinitionData collectDefinitionData(const clang::FunctionDecl& function,
                                     clang::ASTContext& context,
                                     const ParameterMap& parameterMap,
                                     const OptionalCall& call);

std::string makeAbsolute(const std::string& filename);

[[noreturn]] void error(const char* message);
[[noreturn]] void error(llvm::Twine&& twine);

}  // namespace ClangExpand::Routines


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
