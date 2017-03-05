// Library includes
#include "clang-expand/common/state.hpp"
#include "clang-expand/symbol-search/tool-factory.hpp"
#include "clang-expand/definition-search/tool-factory.hpp"

// Clang includes
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

// LLVM includes
#include "llvm/Support/CommandLine.h"

// Standard includes
#include <algorithm>
#include <cstdlib>
#include <functional>
#include <string>
#include <vector>

namespace {
llvm::cl::OptionCategory ClangExpandCategory("minus-tool options");

llvm::cl::extrahelp ClangExpandCategoryHelp(R"()");

llvm::cl::opt<std::string>
    FileOption("file",
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

template <typename Range, typename T>
auto filterOne(const Range& input, const T& needle) {
  std::vector<std::string> rest;
  rest.reserve(input.size() - 1);

  // clang-format off
  std::copy_if(input.begin(), input.end(), rest.begin(),
    [&needle] (const auto& element) {
      return element != needle;
  });
  // clang-format on

  return rest;
}
}  // namespace

auto main(int argc, const char* argv[]) -> int {
  using namespace clang::tooling;  // NOLINT(build/namespaces)

  CommonOptionsParser OptionsParser(argc, argv, ClangExpandCategory);
  const auto& sources = OptionsParser.getSourcePathList();
  if (FileOption.empty()) {
    FileOption = sources.front();
  }

  ClangTool SymbolSearch(OptionsParser.getCompilations(), {FileOption});
  ClangExpand::State state;

  // clang-format off
  auto error =
    SymbolSearch.run(new ClangExpand::SymbolSearch::ToolFactory(
      FileOption, LineOption, ColumnOption,
      [&state] (auto&& result) {
          state = std::move(result);
      }));
  // clang-format on

  if (error) return error;

  if (auto* definition = std::get_if<ClangExpand::DefinitionState>(&state)) {
    llvm::outs() << definition->code << '\n';
    return EXIT_SUCCESS;
  }

  const auto rest = filterOne(sources, FileOption);
  ClangTool DefinitionSearch(OptionsParser.getCompilations(), rest);

  // clang-format off
  error = DefinitionSearch.run(
      new ClangExpand::DefinitionSearch::ToolFactory(
        FileOption, std::get<ClangExpand::DeclarationState>(state),
        [&state](auto&& result) {
          state = std::move(result);
      }));
  // clang-format on

  if (error) return error;

  llvm::outs() << std::get<ClangExpand::DefinitionState>(state).code << '\n';
}
