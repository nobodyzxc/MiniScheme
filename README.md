# Zekin (developing)

To learn how to make an interpreter , I started the project.

zekin is a naive scheme interpreter wrote in C.

Usage :
   chmod a+x zekin
   ./zekin # will compile automatically

feature :

   1. slow TCO (fixed after replacing zipped\_env as zip\_env)
   2. weird macro expansion (may solve it after I finishing SICP)
   3. lack of char type.
   4. 8 bytes integer.
   5. autoload some functions defed in lib.ss
   6. weak error handling (maybe the last thing I'll do)

buildins :

   - \+
   - \-
   - \*
   - /
   - % (mod)
   - =
   - <
   - \>
   - <=
   - \>=
   - not
   - cons
   - car
   - cdr
   - length
   - display
   - source (source a scheme script)
   - void
   - void?
   - null?
   - eq?
   - eqv?
   - equal?
   - symbol?
   - procedure?
   - list?
   - pair?
   - read
   - apply

syntax :

   - if
   - quote
   - lambda
   - define
   - set!

library :

   - list
   - append
   - map
   - fold
   - filter
   - caar
   - cadr
   - caddr
   - cadddr
   - tab (show global env)
   - newline
   - assoc
   - indexof (elt in list)
   - or
   - and
   - let
   - begin
   - cond
   - case
