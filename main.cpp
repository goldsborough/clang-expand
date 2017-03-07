// Project includes
#include "clang-expand/search.hpp"

// Clang includes
#include <clang/Tooling/CommonOptionsParser.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <string>
#include <variant>
#include <vector>

namespace {
llvm::cl::OptionCategory clangExpandCategory("minus-tool options");

llvm::cl::extrahelp clangExpandCategoryHelp(R"()");

llvm::cl::opt<std::string>
    fileOption("file",
               llvm::cl::desc("The source file of the function to expand"),
               llvm::cl::cat(clangExpandCategory));

llvm::cl::opt<unsigned>
    lineOption("line",
               llvm::cl::Required,
               llvm::cl::desc("The line number of the function to expand"),
               llvm::cl::cat(clangExpandCategory));

llvm::cl::opt<unsigned>
    columnOption("column",
                 llvm::cl::Required,
                 llvm::cl::desc("The column number of the function to expand"),
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

  ClangExpand::Search search(fileOption, lineOption, columnOption);
  const auto result = search.run(db, sources);

  llvm::outs() << result.code << '\n';
}


// Output json consisting of validity heuristic (depending on return value)
// range to replace (initializer + function call)
// text to replace with (return value declaration + body)
