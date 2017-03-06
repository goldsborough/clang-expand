// Library includes
#include "clang-expand/search.hpp"

// Clang includes
#include <clang/Tooling/CommonOptionsParser.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>

// Standard includes
#include <iosfwd>
#include <string>
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

  return ClangExpand::Search(fileOption, lineOption, columnOption)
      .run(db, sources);
}
