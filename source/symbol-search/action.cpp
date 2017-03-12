// Project includes
#include "clang-expand/symbol-search/action.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"
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

// LLVM includes
#include <llvm/ADT/StringSet.h>
#include <llvm/ADT/Twine.h>

// Standard includes
#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <string>

namespace ClangExpand::SymbolSearch {
namespace {
bool verifyToken(bool errorOccurred, const clang::Token& token) {
  static const llvm::StringSet<> operatorTokens = {
      "amp",                  // &
      "ampamp",               // &&
      "ampequal",             // &=
      "star",                 // *
      "starequal",            // *=
      "plus",                 // +
      "plusequal",            // +=
      "minus",                // -
      "minusminus",           // --
      "minusequal",           // -=
      "tilde",                // ~
      "exclaim",              // !
      "exclaimequal",         // !=
      "slash",                // /
      "slashequal",           // /=
      "percent",              // %
      "percentequal",         // %=
      "less",                 // <
      "lessless",             // <<
      "lessequal",            // <=
      "lesslessequal",        // <<=
      "greater",              // >
      "greatergreater",       // >>
      "greaterequal",         // >=
      "greatergreaterequal",  // >>=
      "caret",                // ^
      "caretequal",           // ^=
      "pipe",                 // |
      "pipepipe",             // ||
      "pipeequal",            // |=
      "equalequal"            // ==
  };

  if (errorOccurred) {
    Routines::error("Error lexing token at given location");
  }

  if (token.is(clang::tok::raw_identifier)) return false;
  if (operatorTokens.count(token.getName())) return true;

  Routines::error("Token at given location is not an identifier");
}
}  // namespace

Action::Action(const Location& targetLocation, Query& query)
: _query(query), _targetLocation(targetLocation) {
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
    Routines::error("Location is not valid");
  }

  const auto startLocation = clang::Lexer::GetBeginningOfToken(location,
                                                               sourceManager,
                                                               languageOptions);

  if (startLocation.isInvalid()) {
    Routines::error("Error retrieving start of token");
  }

  clang::Token token;
  bool errorOccurred = clang::Lexer::getRawToken(startLocation,
                                                 token,
                                                 sourceManager,
                                                 languageOptions,
                                                 /*IgnoreWhiteSpace=*/true);

  bool isOperator = verifyToken(errorOccurred, token);

  // Good to go.
  _callLocation = startLocation;
  _spelling = clang::Lexer::getSpelling(token, sourceManager, languageOptions);

  if (isOperator) _spelling = "operator" + _spelling;

  // clang-format off
  auto hooks = std::make_unique<MacroSearch>(
    compiler, _callLocation,
    [this](auto&& definition) {
      _query.definition = std::move(definition);
    });
  // clang-format on
  compiler.getPreprocessor().addPPCallbacks(std::move(hooks));

  return true;
}

Action::ASTConsumerPointer
Action::CreateASTConsumer(clang::CompilerInstance&, llvm::StringRef) {
  return std::make_unique<Consumer>(_callLocation, _spelling, _query);
}

clang::FileID Action::_getFileID(clang::SourceManager& sourceManager) const {
  auto& fileManager = sourceManager.getFileManager();
  const auto* fileEntry = fileManager.getFile(_targetLocation.filename);
  if (fileEntry == nullptr || !fileEntry->isValid()) {
    Routines::error("Could not find file " +
                    llvm::Twine(_targetLocation.filename) +
                    " in file manager\n");
  }

  assert(fileEntry->getName() == _targetLocation.filename &&
         "Symbol search should only run on the target TU");

  const auto fileID =
      sourceManager.getOrCreateFileID(fileEntry, clang::SrcMgr::C_User);
  if (!fileID.isValid()) {
    Routines::error("Error getting file ID from file entry");
  }

  return fileID;
}

}  // namespace ClangExpand::SymbolSearch
