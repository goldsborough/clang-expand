// Project includes
#include "clang-expand/common/routines.hpp"
#include "clang-expand/common/canonical-location.hpp"

// Clang includes
#include <clang/AST/ASTContext.h>
#include <clang/Basic/SourceLocation.h>
#include <clang/Rewrite/Core/Rewriter.h>

// LLVM includes
#include <llvm/ADT/SmallString.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <cassert>
#include <cstdlib>
#include <string>
#include <system_error>

namespace ClangExpand::Routines {
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager) {
  return CanonicalLocation(first, sourceManager) ==
         CanonicalLocation(second, sourceManager);
}

std::string getSourceText(const clang::SourceRange& range,
                          clang::SourceManager& sourceManager,
                          const clang::LangOptions& languageOptions) {
  clang::Rewriter rewriter(sourceManager, languageOptions);
  return rewriter.getRewrittenText(range);
}

std::string
getSourceText(const clang::SourceRange& range, clang::ASTContext& context) {
  return getSourceText(range,
                       context.getSourceManager(),
                       context.getLangOpts());
}

std::string makeAbsolute(const std::string& filename) {
  llvm::SmallString<256> absolutePath(filename);
  const auto failure = llvm::sys::path::remove_dots(absolutePath, true);
  assert(!failure && "Error cleaning path before making it absolute");
  (void)failure;
  const auto error = llvm::sys::fs::make_absolute(absolutePath);
  assert(!error && "Error generating absolute path");
  (void)error;
  return absolutePath.str();
}

void error(const char* message) {
  llvm::errs() << message << '\n';
  std::exit(EXIT_FAILURE);
}

void error(llvm::Twine&& twine) {
  llvm::errs() << twine.str() << '\n';
  std::exit(EXIT_FAILURE);
}

}  // namespace ClangExpand::Routines
