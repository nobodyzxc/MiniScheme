# Zekin (developing)

To learn how to make an interpreter , I started the project.

zekin is a naive scheme interpreter wrote in C.

### Usage :
```shell
   chmod a+x zekin
   ./zekin # will compile automatically
```

### Final Goal :

> Run repl.ss by using zekin

### Todo :

   1. consider syntax expansion speed (cond vs if) [ done ]
   2. consider the expressions below
   ```scheme
      (define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
      (f 100000) ; Can I opt it to tail call? (Do I need CPS?)
   ```

### Feature :

   1. TCO
   2. naive macro expansion (may improve it after I finishing SICP)
   3. lack of char type.
   4. 8 bytes integer.
   5. autoload some functions defed in lib.ss
   6. weak error handling (maybe the last thing I'll do)
   7. share buffer (`-DSHARE_BUFFER` in Makefile) or not share buffer
   8. multi-line comment (`#| bala bala |#`)

###  Reference :

#### Syntax Forms

```scheme
   (if test-expr then-expr [else-expr])
```
```scheme
   'datum
   (quote datum)
```
```scheme
   (lambda kw-formals body ...)
   kw-formals = (id ...)
              | (id ... . rest-id)
              | rest-id

```
```scheme
   (define id expr)
   (define (head args) body ...)

   head = id
        | (head args)

   args = arg ...
        | arg ... . rest-id

    arg = arg-id
```
```scheme
   (set! id expr)
```

#### Macro Forms

```scheme
   (and expr ...)
```
```scheme
   (or expr ...)
```
```scheme
   (let ([id val-expr] ...) body ...+)
```
```scheme
   (begin expr ...)
```
```scheme
   (cond cond-clause ...)

   cond-clause = (test-expr then-body ...)
               | (else then-body ...)
               | (test-expr => proc-expr)
               | (test-expr)
               | test-expr    ; my special define
```
```scheme
   (case val-expr case-clause ...)

   case-clause = ([datum ...) then-body ...)
               | (else then-body ...)
```

#### Built-in Functions

```scheme
   (void? v) -> boolean?
   v : any/c
```
```scheme
   (null? v) -> boolean?
   v : any/c
```
```scheme
   (pair? v) -> boolean?
   v : any/c
```
```scheme
   (list? v) -> boolean?
   v : any/c
```
```scheme
   (symbol? v) -> boolean?
   v : any/c
```
```scheme
   (number? v) -> boolean?
   v : any/c
```
```scheme
   (exact? v) -> boolean?
   v : any/c
```
```scheme
   (integer? v) -> boolean?
   v : any/c
```
```scheme
   (string? v) -> boolean?
   v : any/c
```
```scheme
   (procedure? v) -> boolean?
   v : any/c
```
```scheme
   (closure? v) -> boolean?
   v : any/c
```
```scheme
   (env? v) -> boolean?
   v : any/c
```
```scheme
   (eq? v1 v2) -> boolean?
   v1 : any/c
   v2 : any/c
```
```scheme
   (eqv? v1 v2) -> boolean?
   v1 : any/c
   v2 : any/c
```
```scheme
   (equal? v1 v2) -> boolean?
   v1 : any/c
   v2 : any/c
```

```scheme
   (void z ...) -> void?
   z : any/c
```
```scheme
   (not v) -> boolean?
   v : any/c
```
```scheme
   (cons a d) -> pair?
   a : any/c
   d : any/c
```
```scheme
   (car p) -> any/c
   p : pair?
```
```scheme
   (cdr p) -> any/c
   p : pair?
```
```scheme
   (length lst) -> integer?
   lst : list?
```
```scheme
   (= z w ...) -> boolean?
   z : number?
   w : number?
```
```scheme
   (< x y ...) -> boolean?
   x : number?
   y : number?
```
```scheme
   (> x y ...) -> boolean?
   x : number?
   y : number?
```
```scheme
   (<= x y ...) -> boolean?
   x : number?
   y : number?
```
```scheme
   (>= x y ...) -> boolean?
   x : number?
   y : number?
```
```scheme
   (+ z ...) -> number?
   z : number?
```
```scheme
   (- z ...) -> number?
   z : number?
```
```scheme
   (* z ...) -> number?
   z : number?
```
```scheme
   (/ z ...) -> number?
   z : number? ; cannot be zero
```
```scheme
   (% z ...) -> number?
   (mod z ...) -> number?
   z : number? ; cannot be zero
```
```scheme
   (display datum) -> void?
   datum : any/c
```
```scheme
   (source script) -> void?
   script : string?
```
```scheme
   (read) -> any/c
```
```scheme
   (apply proc v) -> any
   proc : procedure?
   v : any/c
```
```scheme
   (get-env clos) -> env?
   clos : closure?
```

#### Library Functions


```scheme
   (list e ...) -> list?
   e : any/c
```
```scheme
   append not ready yet
```
```scheme
   (map proc e ...) -> any/c
   proc : procedure?
   e : list?
```
```scheme
   (fold proc e) -> any/c
   proc : procedure?
   e : list?
```
```scheme
   (filter proc e) -> any/c
   proc : procedure?
   e : list?
```
```scheme
   caar
   cadr
   caddr
   cadddr
   cddr
```
```scheme
   (newline) -> void?
```
```scheme
   (assoc v ls) -> (or/c pair? #f)
   v : any/c
   ls : list?
```
```scheme
   (indexof v ls) -> integer?
   v : any/c
   ls : list?
```
