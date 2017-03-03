#ifndef CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP
#define CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP

// Clang includes
#include "clang/Frontend/FrontendAction.h"
#include "clang/Lex/Token.h"

// LLVM includes
#include "llvm/ADT/StringRef.h"

// Standard includes
#include <functional>
#include <memory>
#include <string>

namespace clang {
class CompilerInstance;
class FileID;
class SourceManager;
class MacroInfo;
}

namespace llvm {
class StringRef;
}

namespace ClangExpand {
struct FunctionProperties;
}

namespace ClangExpand::SymbolSearch {

class FunctionProperties;

class Action : public clang::ASTFrontendAction {
 public:
  using super = clang::ASTFrontendAction;
  using ASTConsumerPointer = std::unique_ptr<clang::ASTConsumer>;
  using ResultCallback =
      std::function<void(const ClangExpand::FunctionProperties&)>;

  Action(const llvm::StringRef& filename,
         unsigned line,
         unsigned column,
         const ResultCallback& resultCallback);

  bool BeginInvocation(clang::CompilerInstance& compiler) override;

  bool BeginSourceFileAction(clang::CompilerInstance& compiler,
                             llvm::StringRef filename) override;

  /// Creates the Consumer instance, forwarding the command line options.
  ASTConsumerPointer CreateASTConsumer(clang::CompilerInstance& compiler,
                                       llvm::StringRef filename) override;

 private:
  clang::FileID _getFileID(clang::SourceManager& sourceManager) const;

  ResultCallback _resultCallback;
  clang::SourceLocation _callLocation;
  std::string _spelling;
  const clang::MacroInfo* _macro;
  std::string _filename;
  unsigned _line;
  unsigned _column;
};

}  // namespace ClangExpand::SymbolSearch

#endif  // CLANG_EXPAND_SYMBOL_SEARCH_ACTION_HPP
