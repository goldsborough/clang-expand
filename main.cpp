// Project includes
#include "clang-expand/result.hpp"
#include "clang-expand/search.hpp"

// Clang includes
#include <clang/Tooling/CommonOptionsParser.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/YAMLTraits.h>
#include <llvm/Support/raw_ostream.h>

// Standard includes
#include <string>
#include <vector>

namespace {
llvm::cl::OptionCategory clangExpandCategory("clang-expand options");

llvm::cl::extrahelp clangExpandCategoryHelp(R"(
Retrieves function, method, operator or macro definitions and optionally
performs automatic parameter replacement. Allows for happy refactoring while
solving the "I don't want to jump around all the time" problem.
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
    rewriteOption("rewrite",
                  llvm::cl::init(true),
                  llvm::cl::desc("Whether to also generate the rewritten "
                                 "definition with parameters replaced"),
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

  ClangExpand::Search search(fileOption,
                             lineOption,
                             columnOption,
                             rewriteOption);
  auto result = search.run(db, sources);

  llvm::yaml::Output yaml(llvm::outs(), /*context=*/nullptr, /*WrapColumn=*/0);
  yaml << result;
}


// Output json consisting of validity heuristic (depending on return value)
// range to rewrite (initializer + function call)
// text to rewrite with (return value declaration + body)
