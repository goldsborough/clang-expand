// Library includes
#include "clang-expand/action.hpp"

// Clang includes
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include "llvm/Support/CommandLine.h"

// Standard includes
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
    RowOption("column",
              llvm::cl::Required,
              llvm::cl::desc("The column number of the function to expand"),
              llvm::cl::cat(ClangExpandCategory));

llvm::cl::extrahelp
    CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace

namespace ClangExpand {
/// A custom \c FrontendActionFactory so that we can pass the options
/// to the constructor of the tool.
struct ToolFactory : public clang::tooling::FrontendActionFactory {
  clang::FrontendAction* create() override {
    return new ClangExpand::Action(FileOption, LineOption, RowOption);
  }
};
}

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;  // NOLINT(build/namespaces)

  CommonOptionsParser OptionsParser(argc, argv, ClangExpandCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  return Tool.run(new ClangExpand::ToolFactory());
}
