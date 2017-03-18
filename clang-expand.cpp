// Project includes
#include "clang-expand/options.hpp"
#include "clang-expand/result.hpp"
#include "clang-expand/search.hpp"

// Third-party includes
#include <third-party/json.hpp>

// Clang includes
#include <clang/Tooling/CommonOptionsParser.h>

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
