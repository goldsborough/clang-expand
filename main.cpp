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

llvm::cl::opt<unsigned>
    LineOption("line",
               llvm::cl::desc("The line number of the function to expand"),
               llvm::cl::cat(ClangExpandCategory));

llvm::cl::opt<unsigned>
    RowOption("column",
              llvm::cl::desc("The column number of the function to expand"),
              llvm::cl::cat(ClangExpandCategory));

llvm::cl::opt<std::string> LocationOption(
    "location",
    llvm::cl::desc("A string in 'line:column' format, specifying "
                   "the -line and -column arguments."),
    llvm::cl::cat(ClangExpandCategory));

llvm::cl::extrahelp
    CommonHelp(clang::tooling::CommonOptionsParser::HelpMessage);
}  // namespace

/// A custom \c FrontendActionFactory so that we can pass the options
/// to the constructor of the tool.
struct ToolFactory : public clang::tooling::FrontendActionFactory {
  clang::FrontendAction* create() override {
    return new ClangExpand::Action();
  }
};

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;

  CommonOptionsParser OptionsParser(argc, argv, ClangExpandCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  // return Tool.run(new ToolFactory());
}
