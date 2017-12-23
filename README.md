# Zekin (developing)

To learn how to make an interpreter , I started the project.

zekin is a naive scheme interpreter wrote in C.

feature :

   1. slow TCO
   2. stupid gc
   3. weird macro expansion
   4. lack of char type.
   5. 4 bytes integer
   6. autoload some functions defed in lib.ss
   7. weak error handling

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
