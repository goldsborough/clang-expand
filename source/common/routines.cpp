//===----------------------------------------------------------------------===//
//
//                           The MIT License (MIT)
//                    Copyright (c) 2017 Peter Goldsborough
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
//===----------------------------------------------------------------------===//

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

namespace ClangExpand {
namespace Routines {
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

std::string getSourceText(const clang::SourceRange& range,
                          clang::ASTContext& context) {
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
  return absolutePath.str().str();
}

void error(const char* message) {
  llvm::errs() << message << '\n';
  std::exit(EXIT_FAILURE);
}

void error(llvm::Twine&& twine) {
  llvm::errs() << twine.str() << '\n';
  std::exit(EXIT_FAILURE);
}

}  // namespace Routines
}  // namespace ClangExpand
