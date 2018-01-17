# Zekin (developing)

To learn how to make an interpreter , I started the project.

zekin is a naive scheme interpreter wrote in C.

Usage :
```shell
   chmod a+x zekin
   ./zekin # will compile automatically
```

final goal :

#### Run repl.ss by using zekin

todo :

   1. consider syntax expansion speed (cond vs if) [ done ]
   2. consider the expressions below
   ```scheme
      (define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
      (f 100000)
   ```
      can I opt it to tail call?

feature :

   1. TCO
   2. naive macro expansion (may improve it after I finishing SICP)
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
