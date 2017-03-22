//===----------------------------------------------------------------------===//
//          _                                                         _
//         | |                                                       | |
//      ___| | __ _ _ __   __ _ ______ _____  ___ __   __ _ _ __   __| |
//     / __| |/ _` | '_ \ / _` |______/ _ \ \/ / '_ \ / _` | '_ \ / _` |
//    | (__| | (_| | | | | (_| |     |  __/>  <| |_) | (_| | | | | (_| |
//     \___|_|\__,_|_| |_|\__, |      \___/_/\_\ .__/ \__,_|_| |_|\__,_|
//                         __/ |               | |
//                        |___/                |_|
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
#include "clang-expand/options.hpp"
#include "clang-expand/result.hpp"
#include "clang-expand/search.hpp"

// Third-party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/Tooling/CommonOptionsParser.h>
#include <clang/Tooling/CompilationDatabase.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <string>
#include <vector>

namespace {
llvm::cl::OptionCategory clangExpandCategory("clang-expand options");

llvm::cl::extrahelp clangExpandCategoryHelp(R"(
Retrieves function, method, operator or macro definitions and optionally
performs automatic parameter replacement. Allows for happy refactoring without
source file gymnastics.
)");

llvm::cl::opt<std::string>
    fileOption("file",
               llvm::cl::desc("The source file of the function to expand"),
               llvm::cl::cat(clangExpandCategory));
llvm::cl::alias fileShortOption("f",
                                llvm::cl::desc("Alias for -file"),
                                llvm::cl::aliasopt(fileOption));

llvm::cl::opt<unsigned>
    lineOption("line",
               llvm::cl::Required,
               llvm::cl::desc("The line number of the function to expand"),
               llvm::cl::cat(clangExpandCategory));
llvm::cl::alias lineShortOption("l",
                                llvm::cl::desc("Alias for -line"),
                                llvm::cl::aliasopt(lineOption));

llvm::cl::opt<unsigned>
    columnOption("column",
                 llvm::cl::Required,
                 llvm::cl::desc("The column number of the function to expand"),
                 llvm::cl::cat(clangExpandCategory));
llvm::cl::alias columnShortOption("c",
                                  llvm::cl::desc("Alias for -column"),
                                  llvm::cl::aliasopt(columnOption));

llvm::cl::opt<bool>
    callOption("call",
               llvm::cl::init(true),
               llvm::cl::desc("Whether to return the source range of the call"),
               llvm::cl::cat(clangExpandCategory));

llvm::cl::opt<bool> declarationOption(
    "declaration",
    llvm::cl::init(true),
    llvm::cl::desc("Whether to return the original declaration"),
    llvm::cl::cat(clangExpandCategory));

llvm::cl::opt<bool> definitionOption(
    "definition",
    llvm::cl::init(true),
    llvm::cl::desc("Whether to return the original definition"),
    llvm::cl::cat(clangExpandCategory));

llvm::cl::opt<bool> rewriteOption(
    "rewrite",
    llvm::cl::init(true),
    llvm::cl::desc("Whether to generate the rewritten (expand) definition"),
    llvm::cl::cat(clangExpandCategory));

llvm::cl::extrahelp
    commonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;  // NOLINT(build/namespaces)

  CommonOptionsParser options(argc, argv, clangExpandCategory);
  const auto& sources = options.getSourcePathList();
  auto& db = options.getCompilations();

  if (fileOption.empty()) {
    fileOption = sources.front();
  }

  // clang-format off
  ClangExpand::Search search(fileOption, lineOption, columnOption);
  auto result = search.run(db, sources, {
    callOption,
    declarationOption,
    definitionOption,
    rewriteOption
  });
  // clang-format on

  llvm::outs() << result.toJson().dump(2) << '\n';
}
