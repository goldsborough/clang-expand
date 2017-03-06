// Project includes
#include "clang-expand/symbol-search/action.hpp"
#include "clang-expand/symbol-search/consumer.hpp"
#include "clang-expand/symbol-search/macro-search.hpp"

// Clang includes
#include <clang/Basic/SourceManager.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Lexer.h>
#include <clang/Lex/MacroInfo.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/Token.h>

// Standard includes
#include <cstdlib>
#include <memory>
#include <string>

namespace ClangExpand::SymbolSearch {
namespace {
void error(const char* message) {
  llvm::errs() << message << "\n";
  std::exit(EXIT_FAILURE);
}

void verifyToken(bool errorOccurred, const clang::Token& token) {
  if (errorOccurred) {
    error("Error lexing token at given location");
  }

  if (!token.is(clang::tok::raw_identifier)) {
    error("Token at given location is not an identifier");
  }
}
}  // namespace

Action::Action(const EasyLocation& targetLocation,
               const StateCallback& stateCallback)
: _stateCallback(stateCallback)
, _alreadyFoundMacro(false)
, _targetLocation(targetLocation) {
}

bool Action::BeginInvocation(clang::CompilerInstance& compiler) {
  return true;
}

bool Action::BeginSourceFileAction(clang::CompilerInstance& compiler,
                                   llvm::StringRef filename) {
  if (!super::BeginSourceFileAction(compiler, filename)) return false;

  auto& sourceManager = compiler.getSourceManager();
  const auto& languageOptions = compiler.getLangOpts();

  const auto fileID = _getFileID(sourceManager);
  const auto line = _targetLocation.offset.line;
  const auto column = _targetLocation.offset.column;
  const auto location = sourceManager.translateLineCol(fileID, line, column);

  if (location.isInvalid()) {
    error("Location is not valid");
  }

  const auto startLocation = clang::Lexer::GetBeginningOfToken(location,
                                                               sourceManager,
                                                               languageOptions);

  if (startLocation.isInvalid()) {
    error("Error retrieving start of token");
  }

  clang::Token token;
  bool errorOccurred = clang::Lexer::getRawToken(startLocation,
                                                 token,
                                                 sourceManager,
                                                 languageOptions,
                                                 /*IgnoreWhiteSpace=*/true);

  verifyToken(errorOccurred, token);

  // Good to go.
  _callLocation = startLocation;
  _spelling = clang::Lexer::getSpelling(token, sourceManager, languageOptions);

  // clang-format off
  auto hooks = std::make_unique<MacroSearch>(
    compiler, _callLocation,
    [this] (auto&& definition) {
      _alreadyFoundMacro = true;
      _stateCallback(std::move(definition));
    });
  // clang-format on

  compiler.getPreprocessor().addPPCallbacks(std::move(hooks));

  return true;
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) {
  // We already have the token at this point, because BeginInvocation is
  // guaranteed to be called before this method. We don't have the macro pointer
  // yet (if ever), because we get it in the preprocessor hooks, which are
  // executed after CreateASTConsumer. So we pass a function for lazy
  // evaluation.
  return std::make_unique<Consumer>(_callLocation,
                                    _spelling,
                                    [this] { return _alreadyFoundMacro; },
                                    _stateCallback);
}

clang::FileID Action::_getFileID(clang::SourceManager& sourceManager) const {
  auto& fileManager = sourceManager.getFileManager();
  const auto* fileEntry = fileManager.getFile(_targetLocation.filename);
  if (fileEntry == nullptr || !fileEntry->isValid()) {
    llvm::errs() << "Could not find file " << _targetLocation.filename
                 << " in file manager\n";
    std::exit(EXIT_FAILURE);
  }

  assert(fileEntry->getName() == _targetLocation.filename &&
         "Symbol search should only run on the target TU");

  const auto fileID =
      sourceManager.getOrCreateFileID(fileEntry, clang::SrcMgr::C_User);
  if (!fileID.isValid()) {
    llvm::errs() << "Error getting file ID from file entry\n";
    std::exit(EXIT_FAILURE);
  }

  return fileID;
}

}  // namespace ClangExpand::SymbolSearch
