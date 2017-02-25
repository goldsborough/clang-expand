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

llvm::cl::opt<bool>
    RewriteOption("rewrite",
                  llvm::cl::init(false),
                  llvm::cl::desc("If set, emits rewritten source code"),
                  llvm::cl::cat(ClangExpandCategory));

llvm::cl::opt<std::string> RewriteSuffixOption(
    "rewrite-suffix",
    llvm::cl::desc("If -rewrite is set, changes will be rewritten to a file "
                   "with the same name, but this suffix"),
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

  return Tool.run(new ToolFactory());
}
