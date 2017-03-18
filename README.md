# clang-expand

![Here be picture](demo.gif)

A clang tool for happy refactoring without source-code gymnastics.

## Overview

I recently overheard the following conversation on my way to work, that may seem familiar to you:

> Gandalf: It is important to refactor your code and keep functions concise and coherent.
> Harry Potter: Yeah, sure, but I hate having to jump around between files to get the full picture of what my code is doing. One function = one place to look.
> Obi Wan Kenobi: Use the force, Harry.
> Gandalf: He means *clang-expand* :sparkles:

Inspired by Gandalf's words, I set out to find a solution to Harry's problem and built *clang-expand*. Point it at a function invocation in your source code and tell it where to look for stuff, and it will find the correct definition of that particular (template) function, method, operator overload or even constructor and "expand" it into the current scope. *Expanding* means it will:

1. Replace parameters with respective argument expressions. That is, for a
function `f(int x)` that you call with `f(5)`, clang-expand will rewrite every
occurrence of `x` inside `f` to 5. Note that since clang-expand uses clang, it
actually understands C++ and knows what occurrences of `x` are parameter
references and what aren't.

2. If the you're assigning the return value of a function you expand to a
variable, clang-expand will replace every `return` statement inside the function
with an assignment. It attempts to do this in a reasonably intelligent way,
constructing the variable with the return value directly if there is only one
`return` and else first declaring the variable and then assigning. The latter
only works if the type of the variable is default-constructible and clang-expand
will refuse to expand otherwise.

<table
<tr><th>Given</th></tr>
<tr>
<td valign="top" colspan="2">
<pre lang="cpp">
std::string concat(const std::string& first, const std::string& second) {
  return first + "-" + second;
}
</pre>
</td>
</tr>
<tr><th><b>Unexpanded</b></th><th><b>Expanded</b></th></tr>
<tr>
<td valign="top">
<pre lang="cpp">
auto string = concat("clang", "expand");
              ^
</pre>
</td>
<td valign="top">
<pre lang="cpp">
std::__1::basic_string<char> string = \"clang\" + \"-\" + \"expand\";
</pre>
</td></tr>
</table>
