// Library includes
#include "clang-expand/symbol-search/tool-factory.hpp"
#include "clang-expand/common/state.hpp"

// Clang includes
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include "llvm/Support/CommandLine.h"

// Standard includes
#include <functional>
#include <string>

namespace {
llvm::cl::OptionCategory ClangExpandCategory("minus-tool options");

llvm::cl::extrahelp ClangExpandCategoryHelp(R"()");

llvm::cl::opt<std::string>
    FileOption("file",
               llvm::cl::Required,
               llvm::cl::desc("The source file of the function to expand"),
               llvm::cl::cat(ClangExpandCategory));

llvm::cl::opt<unsigned>
    LineOption("line",
               llvm::cl::Required,
               llvm::cl::desc("The line number of the function to expand"),
               llvm::cl::cat(ClangExpandCategory));

llvm::cl::opt<unsigned>
    ColumnOption("column",
              llvm::cl::Required,
              llvm::cl::desc("The column number of the function to expand"),
              llvm::cl::cat(ClangExpandCategory));

llvm::cl::extrahelp
    CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;  // NOLINT(build/namespaces)

  CommonOptionsParser OptionsParser(argc, argv, ClangExpandCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  ClangExpand::State state;

  // clang-format off
  return Tool.run(new ClangExpand::SymbolSearch::ToolFactory(
    FileOption, LineOption, ColumnOption,
    [&state](const auto& result) {
        state = result;
    }));
  // clang-format on
}
