# zekin

Zekin is my scheme inerpreter started from zero.

What should I provide ?

S expression parser (done)

list structure      (done)

environment & closure (done)

garbage collection (done)

from wiki:

Standard forms in the language R5RS Scheme

|Purpose|Forms|
| ------------- | ------------- |
|Definition|define|
|Binding constructs|lambda, do (L), let (L), let\* (L), letrec (L)|
|Conditional evaluation|if, cond (L), case (L), and (L), or (L)|
|Sequential evaluation|begin (\*)|
|Iteration|lambda, do (L), named let (L)|
|Syntactic extension|define-syntax, let-syntax, letrec-syntax, syntax-rules (R5RS), syntax-case (R6RS)|
|Quoting|quote('), unquote(,), quasiquote(`), unquote-splicing(,@)|
|Assignment|set!|
|Delayed evaluation|delay (L)|

Standard procedures in the language R5RS Scheme

|Purpose|Procedures|
| ------------- | ------------- |
|Construction|vector, make-vector, make-string, list|
|Equivalence predicates|eq?, eqv?, equal?, string=?, string-ci=?, char=?, char-ci=?|
|Type conversion|vector->list, list->vector, number->string, string->number, symbol->string, string->symbol, char->integer, integer->char, string->list, list->string|
|Numbers|See separate table|
|Strings|string?, make-string, string, string-length, string-ref, string-set!, string=?, string-ci=?, string<? string-ci<?, string<=? string-ci<=?, string>? string-ci>?, string>=? string-ci>=?, substring, string-append, string->list, list->string, string-copy, string-fill!|
|Characters|char?, char=?, char-ci=?, char<? char-ci<?, char<=? char-ci<=?, char>? char-ci>?, char>=? char-ci>=?, char-alphabetic?, char-numeric?, char-whitespace?, char-upper-case?, char-lower-case?, char->integer, integer->char, char-upcase, char-downcase|
|Vectors|make-vector, vector, vector?, vector-length, vector-ref, vector-set!, vector->list, list->vector, vector-fill!|
|Symbols|symbol->string, string->symbol, symbol?|
|Pairs and lists|pair?, cons, car, cdr, set-car!, set-cdr!, null?, list?, list, length, append, reverse, list-tail, list-ref, memq. memv. member, assq, assv, assoc, list->vector, vector->list, list->string, string->list|
|Identity predicates|boolean?, pair?, symbol?, number?, char?, string?, vector?, port?, procedure?|
|Continuations|call-with-current-continuation (call/cc), values, call-with-values, dynamic-wind|
|Environments|eval, scheme-report-environment, null-environment, interaction-environment (optional)|
|Input/output|display, newline, read, write, read-char, write-char, peek-char, char-ready?, eof-object? open-input-file, open-output-file, close-input-port, close-output-port, input-port?, output-port?, current-input-port, current-output-port, call-with-input-file, call-with-output-file, with-input-from-file(optional), with-output-to-file(optional)|
|System interface|load (optional), transcript-on (optional), transcript-off (optional)|
|Delayed evaluation|force|
|Functional programming|procedure?, apply, map, for-each|
|Booleans|boolean? not|

Standard numeric procedures in the language R5RS Scheme

|Purpose|Procedures|
| ------------- | ------------- |
|Basic arithmetic operators|+, -, *, /, abs, quotient, remainder, modulo, gcd, lcm, expt, sqrt|
|Rational numbers|numerator, denominator, rational?, rationalize|
|Approximation|floor, ceiling, truncate, round|
|Exactness|inexact->exact, exact->inexact, exact?, inexact?|
|Inequalities|<, <= , >, >=, =|
|Miscellaneous predicates|zero?, negative?, positive? odd? even?|
|Maximum and minimum|max, min|
|Trigonometry|sin, cos, tan, asin, acos, atan|
|Exponentials|exp, log|
|Complex numbers|make-rectangular, make-polar, real-part, imag-part, magnitude, angle, complex?|
|Input-output|number->string, string->number|
|Type predicates|integer?, rational?, real?, complex?, number?|

CFG to ref:
```
literals:
<bool>           ::= #t | #f
<id>             ::= <letter>(<letter> | <digit>)*
<symbol>         ::= '<id>
<digit>          ::= 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9

<int>            ::= [ - | + ] <digit>+

<decimal>        ::= <int> .  <digit>+
<num>            ::= <int> | <decimal>

<list-lit>       ::= <id> | <num> | <bool> |  ( <list-lit>* )

<list>           ::= '( <list-lit>* )
<literal>        ::= <bool> | <num> | <symbol> | <list>

expressions:
<define-simple>  ::= (define <id><exp>)
<define-lambda>  ::= (define( <id>+)<exp> )
<define>         ::= <define-simple> | <define-lambda>
<variable>       ::= <id> that is not a reserved word
<application>    ::= (<exp>+)
<if>             ::= (if <exp> <exp> <exp> )
<let>            ::= (let( ( ( <id> <exp> ) )* ) <exp> )
<letrec>         ::= (letrec(  ( ( <id> <exp> ) )* ) <exp> )
<cond>           ::= (cond   (  (<exp> <exp> ) )* [ (else <exp> )  ]  )
<lambda>         ::= (lambda( <id>* )  <exp> )
<exp>            ::= <literal> | <variable> | <application> | <if>     | <let>
```
