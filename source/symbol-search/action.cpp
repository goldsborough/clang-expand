// Project includes
#include "clang-expand/symbol-search/action.hpp"
#include "clang-expand/common/offset.hpp"
#include "clang-expand/common/query.hpp"
#include "clang-expand/common/routines.hpp"
#include "clang-expand/symbol-search/consumer.hpp"
#include "clang-expand/symbol-search/macro-search.hpp"

// Clang includes
#include <clang/Basic/FileManager.h>
#include <clang/Basic/SourceManager.h>
#include <clang/Basic/TokenKinds.h>
#include <clang/Frontend/CompilerInstance.h>
#include <clang/Lex/Lexer.h>
#include <clang/Lex/PPCallbacks.h>
#include <clang/Lex/Preprocessor.h>
#include <clang/Lex/Token.h>

// LLVM includes
#include <llvm/ADT/StringRef.h>
#include <llvm/ADT/StringSet.h>
#include <llvm/ADT/Twine.h>

// Standard includes
#include <cassert>
#include <memory>
#include <string>
#include <utility>


namespace ClangExpand::SymbolSearch {
namespace {

/// Makes sure the token under the cursor is something we can handle.
///
/// "Things we can handle" means either (1) identifiers (for functions, macros,
/// methods etc.) or (2) operators (for overloads).
///
/// \returns True if the token is an operator, else false if it is a simple
/// identifier.
bool verifyToken(const clang::Token& token) {
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

  if (token.is(clang::tok::raw_identifier)) return false;
  if (operatorTokens.count(token.getName())) return true;

  Routines::error("Token at given location is not an identifier");
}

/// Attempts to get the `clang::FileID` for the target location.
clang::FileID getFileID(const Location& targetLocation,
                        clang::SourceManager& sourceManager) {
  auto& fileManager = sourceManager.getFileManager();
  const auto* fileEntry = fileManager.getFile(targetLocation.filename);
  if (fileEntry == nullptr || !fileEntry->isValid()) {
    Routines::error("Could not find file " +
                    llvm::Twine(targetLocation.filename) +
                    " in file manager\n");
  }

  assert(fileEntry->getName() == targetLocation.filename &&
         "Symbol search should only run on the target TU");

  const auto fileID =
      sourceManager.getOrCreateFileID(fileEntry, clang::SrcMgr::C_User);
  if (!fileID.isValid()) {
    Routines::error("Error getting file ID from file entry");
  }

  return fileID;
}

/// Translates our friendly representation of a location to a compact
/// `clang::SourceLocation` for further processing with clang APIs.
clang::SourceLocation translateLocation(const Location& location,
                                        clang::SourceManager& sourceManager) {
  const auto fileID = getFileID(location, sourceManager);
  const auto line = location.offset.line;
  const auto column = location.offset.column;
  const auto translated = sourceManager.translateLineCol(fileID, line, column);
  if (translated.isInvalid()) {
    Routines::error("Location is not valid");
  }
  return translated;
}

/// Given a location between the start and end of a token, returns a location
/// for the start of the token.
clang::SourceLocation
getBeginningOfToken(const clang::SourceLocation& somewhere,
                    clang::SourceManager& sourceManager,
                    const clang::LangOptions& languageOptions) {
  const auto startLocation = clang::Lexer::GetBeginningOfToken(somewhere,
                                                               sourceManager,
                                                               languageOptions);

  if (startLocation.isInvalid()) {
    Routines::error("Error retrieving start of token");
  }

  return startLocation;
}

/// Lexes the token at the given location.
clang::Token lex(const clang::SourceLocation& startLocation,
                 clang::SourceManager& sourceManager,
                 const clang::LangOptions& languageOptions) {
  clang::Token token;
  bool errorOccurred = clang::Lexer::getRawToken(startLocation,
                                                 token,
                                                 sourceManager,
                                                 languageOptions,
                                                 /*IgnoreWhiteSpace=*/true);
  if (errorOccurred) {
    Routines::error("Error lexing token at given location");
  }

  return token;
}
}  // namespace

Action::Action(Location targetLocation, Query& query)
: _query(query), _targetLocation(std::move(targetLocation)) {
}

bool Action::BeginSourceFileAction(clang::CompilerInstance& compiler,
                                   llvm::StringRef filename) {
  if (!super::BeginSourceFileAction(compiler, filename)) return false;

  auto& sourceManager = compiler.getSourceManager();
  const clang::SourceLocation location =
      translateLocation(_targetLocation, sourceManager);

  const auto& languageOptions = compiler.getLangOpts();
  const auto& startLocation =
      getBeginningOfToken(location, sourceManager, languageOptions);

  clang::Token token = lex(startLocation, sourceManager, languageOptions);
  bool isOperator = verifyToken(token);

  // Good to go.
  _callLocation = startLocation;
  _spelling = clang::Lexer::getSpelling(token, sourceManager, languageOptions);

  if (isOperator) _spelling = "operator" + _spelling;

  _installMacroFacilities(compiler);

  /// Continue.
  return true;
}

Action::ASTConsumerPointer Action::CreateASTConsumer(clang::CompilerInstance&,
                                                     llvm::StringRef) {
  return std::make_unique<Consumer>(_callLocation, _spelling, _query);
}

void Action::_installMacroFacilities(clang::CompilerInstance& compiler) const {
  auto hooks = std::make_unique<MacroSearch>(compiler, _callLocation, _query);
  compiler.getPreprocessor().addPPCallbacks(std::move(hooks));
}

}  // namespace ClangExpand::SymbolSearch
