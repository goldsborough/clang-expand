#ifndef CLANG_EXPAND_SEARCH_HPP
#define CLANG_EXPAND_SEARCH_HPP

/// \defgroup SymbolSearch
/// \defgroup DefinitionSearch

// Project includes
#include "clang-expand/common/location.hpp"

// Standard includes
#include <string>
#include <vector>

namespace clang {
namespace tooling {
class CompilationDatabase;
}
}

namespace ClangExpand {
struct Query;
struct Result;
struct Options;

/// Represents a single run of the clang-expand tool.
///
/// ## Overview
///
/// A clang-expand invocation is split up into two major phases, that are in
/// fact each one clang tool: "symbol search" and "definition search". The end
/// goal of these two phases is to find the definition of a function (if so
/// requested by the user) and maybe rewrite it.
///
/// ### Symbol Search
///
/// Symbol search is primarily concerned with finding the declaration of a
/// function. It does so in three steps:
///
/// 1. First, it finds the location of the function call in a representation
/// understood by clang (i.e. as a `clang::SourceLocation`). This involves
/// lexing the token "under the cursor" and convincing the source manager to
/// hand over a `SourceLocation`.
///
/// 2. Next, it finds the referenced call expression as a node in the
/// AST. For this, a clang tool is spawned that walks the AST, looking for all
/// function calls with the same name as we are looking for. If we find one that
/// is at the location we want, we have our function call. This
/// `clang::CallExpr` includes a multitude of rich semantic information about
/// the call that we can further make use of.
///
/// 3. The final step is extracting information about the
/// declaration of the function. More precisely, we collect all the data we need
/// so that subsequent phases of clang-expand can match function definitions
/// back to this correct declaration. This data includes the name of the
/// function, the parameter types and all *contexts* (namespaces or struct/class
/// names) up to the `TranslationUnitDecl`.
///
/// Along the way, we may pick up one of two different interesting pieces of
/// information about the function call:
///
/// 1. It may in fact not be a function call, but a *macro invocation*.
/// If this is the case, we do something completely different than otherwise, as
/// we need to hook into clang's preprocessing stage, i.e. we are not interested
/// in the AST at all. Furthermore, the result of this will not be a
/// declaration, but a definition.
///
/// 2. If it is a function call, its declaration may also be a definition.
/// In that case we can not only collect information about the declaration, but
/// also the definition.
///
/// In both of these cases, we have already found the definition of the function
/// (or macro) in the symbol search phase. As the next phase is no longer
/// required, the tool ends its execution at this point and the tool exits.
///
/// Note that the world (C++) is unfortunately a lot more complex than it
/// may seem from this description. Not all function calls are equal, since some
/// may be constructor expressions, binary operators, method calls or function
/// calls nested inside other statements. clang-expand tries to handle as many
/// situations as possible and takes care to match for cases that it knows it
/// can handle correctly and fails otherwise, rather than doing something stupid
/// because it doesn't know better.
///
/// ### Definition Search
///
/// When the symbol search phase does not find a definition but only a
/// declaration, this is most likely because the definition is in some other
/// translation unit that would be linked in at link-time. It is then the
/// responsibility of the definitition search stage to use the information
/// gathered by symbol search about the function declaration to find the
/// corresponding definition. This phase is again a clang tool of its own that
/// takes in a set of source files and looks for matching functions in each
/// file. For each function definition that matches the target declaration by
/// name, its contexts and parameter types are compared to the target
/// declaration.
///
/// Once a definition is found, definition search will collect location and
/// source information about it. Moreover, it is at this point that the function
/// body can be inspected and rewritten to perform *expansion* of the original
/// function call. This is done by initially collecting a *parameter map* in the
/// declaration data that maps paremeter names to argument expressions (already
/// during symbol search). When it comes to rewriting the body, the tool then
/// taverses its AST subtree and looks for all references to parameters of the
/// function. Each such reference is then replaced with the respective argument
/// passed to the function call for that parameter. Additionally, if the
/// function call is an assignment like `int x = f(42);`, return statements will
/// be rewritten to assignments. This may not always be possible, for example
/// when the type of the assigned variable is not default constructible but we
/// find more than one return statement in the function body, meaning we cannot
/// assign to the variable directly. clang-expand will fail with an appropriate
/// error message in such a case.
///
/// There are again many more edge cases and possibilities to handle than
/// outlined here (courtesy of the flexibility of the C++ language). To sum up,
/// however, the methods described above work for the following expressions:
///
/// - simple function calls: `f(5);`
/// - function calls as return statements: `return f(5);`
/// - function calls with assignments: `int x = f(5);`
/// - "external" method calls: `int x = o.f(); int y = p->f();`
/// - "internal" method calls: `void X::f() { this->g(5); }`
/// - operator overloads: `int x = ~o; return x == y;`
/// - value initialization (constructor calls): `X x = X(5);`
///
/// clang-expand tries also tries hard to be smart about how it handles variable
/// declarations for expansions. In particular, if the expanded function body
/// has only one return statements, that `return` will be rewritten to directly
/// initialize the assigned variable. For example:
///
/// ```.cpp
/// int f(int x) {
///   int y = 0;
///   if (x > 10) {
///     y += 1;
///   }
///   return x + y;
/// }
///
/// int z = f(5);
/// ```
///
/// will expand to:
///
/// ```.cpp
/// int y = 0;
/// if (5 > 10) {
///   y += 1;
/// }
/// int z = 5 + y;
/// ```
///
/// while a function with two return statements like:
///
/// ```.cpp
/// int f(int x) {
///  if (rand() == 42) {
///    return x;
///  }
///  return x + 1;
/// }
///
/// int z = f(5);
/// ```
///
/// will expand to:
///
/// ```.cpp
/// int z;
/// if (rand() == 42) {
///   z = 5;
/// }
/// z = 5 + 1;
/// ```
class Search {
 public:
  using CompilationDatabase = clang::tooling::CompilationDatabase;
  using SourceVector = std::vector<std::string>;

  /// Constructs a new `Search` object with the `file`, `line` and `column`
  /// options from the command line.
  Search(const std::string& file, unsigned line, unsigned column);

  /// Runs the search on the given sources and with the given options.
  /// \returns A `Result`, ready to be printed to the console.
  Result run(CompilationDatabase& compilationDatabase,
             const SourceVector& sources,
             const Options& options);

 private:
  /// Performs the symbol search phase. Decorates the `Query` with
  /// `DeclarationData` and `CallData`, as well as possibly `DefinitionData`.
  void _symbolSearch(CompilationDatabase& compilationDatabase, Query& query);

  /// Performs the definition search phase. Decorates the `Query` with
  /// `DefinitionData`.
  void _definitionSearch(CompilationDatabase& compilationDatabase,
                         const SourceVector& sources,
                         Query& query);

  /// The target location, created from the constructor arguments.
  Location _location;
};
}  // namespace ClangExpand

#endif  // CLANG_EXPAND_SEARCH_HPP
