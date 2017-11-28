# zekin

my scheme inerpreter start from zero

What I should provide ?

S expression parser (done)
list structure      (done)
environment & closure

the function/special form I may need to implement.
```
; special forms
define
if
cond
else
lambda
and
begin
let
set!

; boolean funcs
eq?
list?
pair?
null?
symbol?
procedure?
=
not

; structure funcs
car
cdr
cons
list

; special funcs
apply
; read (like eval)

; io funcs
read
display
flush-output
```
The functions I needn't impl but should extend in other scheme file.
```
map
cadr
caddr
cadddr
assoc
newline
length
```

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
                               | <letrec>   | <cond>        | <define> | <lambda>
```
