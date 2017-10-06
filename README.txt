INTRODUCTION:
The Mitten language is a simple programming language.  This code contains an 
interpreter for it.  It is currently a very simple language; I hope to add 
additional features to it in the future.

USAGE: 
Compile the code, and change permissions as necessary.  Pass a single file as an argument 
to the executable.

DESCRIPTION OF THE CODE:
The interpreter begins by constructing an abstract syntax tree.  It then traverses the
tree and executes the Mitten code.  Most of the parsing is done using the recursive
descent technique.  However, when the interpreter encounters a mathematical expression, it 
splits it into tokens and then performs precedence climbing.

MAJOR LIMITATIONS:
1) Currently, the interpreter only supports the float data type.  The exception to this is
   that string literals can be printed.
2) The interpreter is not guaranteed to correctly handle all incorrect Mitten code.  Code
   with incorrect syntax may cause the interpreter to crash.

DESCRIPTION OF THE LANGUAGE SYNTAX:
1) Each statement must be on its own line.  Code may be indented with spaces without
   affecting the interpreter; however, indentation using tabs is not permitted.

2) Type specifications are not required.  To initialize or change a variable’s value, use
   the format var_name = expression.
       Examples: var_1 = 2 + 3
                 var_2 = var_1 ^ 2

3) The following binary operators are supported: + (addition), - (subtraction), 
   * (multiplication), / (division), <= (less than or equal to), < (less than),
   >= (greater than or equal to), > (greater than), ^ (exponent), && (and), || (or)

4) While loops are declared using the syntax while{condition}{ and ended with a } on its
   own line.
       Example: while{a < 10}{
                    a = a + 1
                }
5) One float or string literal may be printed using a single print statement.  To print,
   use the syntax print expression where expression can be a mathematical expression,
   float variable name, or string literal.
       Examples: print 1 + 2 - a
                 print a
                 print “This is text.”
   A newline is printed after each print statement, so each successive printed string or
   expression is on its own line.
6) Each comment must be on its own line.  A comment is begun with two slashes //
