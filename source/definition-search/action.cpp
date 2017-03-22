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
#include "clang-expand/definition-search/action.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/definition-search/consumer.hpp"

// LLVM includes
#include <llvm/ADT/StringRef.h>

// Standard includes
#include <string>

namespace ClangExpand {
namespace DefinitionSearch {
Action::Action(const std::string& declarationFile, Query& query)
: _declarationFile(Routines::makeAbsolute(declarationFile)), _query(query) {
}

Action::ASTConsumerPointer Action::CreateASTConsumer(clang::CompilerInstance&,
                                                     llvm::StringRef filename) {
  // Skip the file we found the declaration in
  if (filename == _declarationFile) return nullptr;
  return std::make_unique<Consumer>(_query);
}

}  // namespace DefinitionSearch
}  // namespace ClangExpand
