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

#ifndef CLANG_EXPAND_COMMON_ROUTINES_HPP
#define CLANG_EXPAND_COMMON_ROUTINES_HPP

// Standard includes
#include <string>

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

namespace ClangExpand {
namespace Routines {

/// Compares two source locations for equality in a way that actually works.
///
/// \see CanonicalLocation
bool locationsAreEqual(const clang::SourceLocation& first,
                       const clang::SourceLocation& second,
                       const clang::SourceManager& sourceManager);

/// Retrieves the raw source text within a range, as a string.
std::string getSourceText(const clang::SourceRange& range,
                          clang::SourceManager& sourceManager,
                          const clang::LangOptions& languageOptions);

/// Retrieves the raw source text within a range, as a string. Passes the source
/// manager and language options from the `ASTContext` to the other overload.
std::string getSourceText(const clang::SourceRange& range,
                          clang::ASTContext& context);

/// Turns a file path into an absolute file path.
std::string makeAbsolute(const std::string& filename);

/// Prints an error message to stderr and exits. the program.
[[noreturn]] void error(const char* message);

/// Prints an error message to stderr and exits. the program. The message is
/// taken as the result of calling `twine.str()`.
[[noreturn]] void error(llvm::Twine&& twine);

}  // namespace Routines
}  // namespace ClangExpand


#endif  // CLANG_EXPAND_COMMON_ROUTINES_HPP
