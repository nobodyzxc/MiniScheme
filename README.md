# MiniScheme
MiniScheme by Racket R5RS(current ver)

What I should provide to replace Racket(R5RS)?

=> naive interp for repl.ss , it should contain

S expression parser
list structure

let , define , cons , car , cdr ... and so on.

the naive interp would be python , c and so on.

the function/special form I may need to impl.
```
; special forms
define
if
else
cond
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
