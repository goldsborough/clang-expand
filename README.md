# clang-expand

![Here be picture](demo.gif)

A clang tool for happy refactoring without source-code gymnastics.

## Overview

I recently overheard the following conversation on my way to work, that may seem familiar to you:

> __Gandalf__: It is important to refactor your code and keep functions concise and coherent.  
> __Harry Potter__: Yeah, sure, but I hate having to jump around between files to get the full picture of what my code is doing. One function = one place to look.  
> __Obi Wan Kenobi__: Use the force, Harry.  
> __Gandalf__: He means *clang-expand* :sparkles:

Inspired by Gandalf's words, I set out to find a solution to Harry's problem and built *clang-expand*. Point it at a function invocation in your source code and tell it where to look for stuff, and it will find the correct definition of that particular (template) function, method, operator overload or even constructor and "expand" it into the current scope. *Expanding* means it will:

1. Replace parameters with respective argument expressions. That is, for a
function `f(int x)` that you call with `f(5)`, clang-expand will rewrite every
occurrence of `x` inside `f` to `5`. Note that since clang-expand uses clang, it
actually understands C++ and knows what occurrences of `x` are parameter
references and what aren't.

<p align="center">
<table align="center">
<tr><th colspan="2">Given</th></tr>
<tr valign="top"><td colspan="2"><sub><pre lang="cpp">
template&lt;typename Range&gt;
void magic(Range& range) {
  auto iterator = std::find(range.begin(), range.end(), 42);
  if (iterator != range.end()) {
    range.erase(iterator);
    std::cout << "Successfully erased all meaning of life\n";
  }
}
</pre></sub></td></tr>
<tr><th>Unexpanded</th><th>Expanded</th></tr>
<tr valign="top">
<td><sub><pre lang="cpp">
std::vector<int> v = {1, 42, 3};                                       &nbsp;
magic(v);
^
</pre></sub></td>
<td><sub><pre lang="cpp">
std::vector<int> v = {1, 42, 3};
auto iterator = std::find(v.begin(), v.end(), 42);
if (iterator != v.end()) {
  std::cout << "Successfully erased all meaning of life\n";
  v.erase(iterator);
}
</pre></sub></td>
</tr>
</table>
</p>

As you can see, clang-expand actually instantiated the template function during
the expansion. This is because on the level that it operates on within the clang
AST, semantic analysis, including template type deduction, are already complete.
This means that calling templates is not a problem for clang-expand.

2. If you're assigning the return value of a function you expand to a
variable, clang-expand will replace every `return` statement inside the function
with an assignment. It attempts to do this in a reasonably intelligent way,
constructing the variable with the return value directly if there is only one
`return` and else first declaring the variable and then assigning. The latter
only works if the type of the variable is default-constructible and clang-expand
will refuse to expand otherwise.

<table>
<tr><th colspan="2">Given</th></tr>
<tr valign="top"><td colspan="2"><sub><pre lang="cpp">
std::string concat(const std::string&amp; first, const std::string&amp; second) {
  return first + "-"s + second;
}
<br>
std::string concat(const std::string&amp; first, const std::string&amp; second, bool kebab) {
  if (kebab) {
    return first + "-"s + second;
  } else {
    return first + std::toupper(second.front(), {}) + second.substr(1);
  }
}
</pre></sub></td></tr>
<tr><th>Unexpanded</th><th>Expanded</th></tr>
<tr valign="top">
<td><sub><pre lang="cpp">
auto kebab = concat("clang", "expand");
             ^
</pre></sub></td>
<td><sub><pre lang="cpp">
std::string kebab = "clang" + "-"s + "expand";
</pre></sub></td>
</tr>
<tr><th>Unexpanded</th><th>Expanded</th></tr>
<tr valign="top">
<td><sub><pre lang="cpp">
auto maybeCamel = concat("clang", "expand", flipCoin());
                  ^
</pre></sub></td>
<td><sub><pre lang="cpp">
std::string maybeCamel;
if (flipCoin()) {
  maybeCamel = "clang" + "-"s + "expand";
} else {
  maybeCamel = "clang" + std::toupper("expand".front(), {}) + "expand".substr(1);
}
</pre></sub></td>
</tr>
</table>

3. If you're calling a method, clang-expand will prepend the base to every method or member of referenced inside:

<table>
<tr><th>Unexpanded</th><th>Expanded<sup><a href="#fn1">1</a></sup></th></tr>
<tr valign="top">
<td><sub><pre lang="cpp">
std::vector<int> my_vec;
my_vec.emplace_back(42);
       ^
</pre></sub></td>
<td><sub><pre lang="cpp">
std::vector<int> my_vec;
if (my_vec.__end_ < my_vec.__end_cap())
{
    __RAII_IncreaseAnnotator __annotator(*this);
    __alloc_traits::construct(my_vec.__alloc(),
                              _VSTD::__to_raw_pointer(my_vec.__end_),
                              _VSTD::forward<_Args>(42)...);
    __annotator.__done();
    ++my_vec.__end_;
}
else
    my_vec.__emplace_back_slow_path(_VSTD::forward<_Args>(42)...);
</pre></sub></td>
</tr>
</table>

<a name="fn1">1</a>: This is the implementation on my system, of course.

4. If the function you're expanding is an operator, clang-expand can handle that in just the same way:

<table>
<tr><th colspan="2">Given</th></tr>
<tr valign="top"><td colspan="2"><sub><pre lang="cpp">
struct by_lightning {
  bool operator==(const by_lightning& other) const noexcept {
    return this->circuit == other.circuit;
  }
  short circuit;
};
</pre></sub></td></tr>
<tr><th>Unexpanded</th><th>Expanded</th></tr>
<tr valign="top">
<td><sub><pre lang="cpp">
by_lightning first{1};
by_lightning second{2};
return first == second;
             ^
</pre></sub></td>
<td><sub><pre lang="cpp">
by_lightning first{1};
by_lightning second{2};
return first.circuit == other.circuit;
</pre></sub></td>
</tr>
</table>
