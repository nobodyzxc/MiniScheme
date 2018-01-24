# Zekin (developing)

To learn how to make an interpreter , I started the project.

zekin is a naive scheme interpreter wrote in C.

### Usage :
```shell
   make

   ./zekin [ -e exprs | [ -i ] scripts | -h ]

   COMMAND LINE OPTIONS
          -e     eval expressions after the flag

          -i     enter interactive mode after load scripts

          -h     display the help information
```

### Goals

> Provide some interesting functions to observe the ecology

> Do some interesting optimizations
(symbol , gc , TCO , any opt about speed and space)

> System calls

> Macro

### Optional Goal

> escape char in string (numberical , like \x \o ...)

> more types , i.e. char , big number (8 byte current) , rational ...

> string operations

> support printf function

### Final Goal :

> Run repl.ss by using zekin

### Consider :

   1. consider tco , when apply a closure ,
      with non-lambda args , can I just update without new a env?

### Todo :

   1. consider syntax expansion speed (cond vs if) [ done ]
   2. consider the expressions below
   ```scheme
      (define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
      (f 100000) ; Can I opt it to tail call? (Do I need CPS?)
   ```
   3. add let family , `let*` , `letrec` ...
   4. figureout `syntax-rules` and improve ( rewrite ) macro

### Feature :

   1. TCO
   2. naive macro expansion (may improve it after I finishing SICP)
   3. autoload some functions defed in lib.ss
   4. weak error handling (maybe the last thing I'll do)
   5. multi-line comment (`#| bala bala |#`)
   6. use GNU readline function in interative mode

###  Reference :

#### Syntax Forms

```scheme
   (if test-expr then-expr else-expr)
   (if test-expr then-expr)
```
```scheme
   'datum
   (quote datum)
```
```scheme
   (lambda params body ...)
   params = (id ...)
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
   (syntax-rules
      (keywords ...)
      (pattern template) ...)

   pattern = (_ match ...)
     match = keywords
           | keywords ellipsis
           | parameter
           | parameter ellipsis
           | (match ...)
           | (match ...) ellipsis

   ellipsis = ...

   ; use define to binding syntax-rules
   ; can ref lib.ss
```
```scheme
   (set! id expr)
```
```scheme
   (set-cdr! expr expr)
```
```scheme
   (set-car! expr expr)
```

#### Macro Forms

```scheme
   (and expr ...)
```
```scheme
   (or expr ...)
```
```scheme
   (let ((id val-expr) ...) body ...+)
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

   case-clause = ((datum ...) then-body ...))
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
```scheme
   (get-curenv v ...) -> env?
   v : any/c
```
```scheme
   (lookup-symbol sym env) -> any
   sym : symbol?
   env : env?
```
```scheme
   (system cmd) -> void?
   cmd : string?
```



#### Library Functions

```scheme
   (caar x)        (car (car x))
   (cadr x)        (car (cdr x))
   (cdar x)        (cdr (car x))
   (cddr x)        (cdr (cdr x))
   (caaar x)       (car (car (car x)))
   (caadr x)       (car (car (cdr x)))
   (cadar x)       (car (cdr (car x)))
   (caddr x)       (car (cdr (cdr x)))
   (cdaar x)       (cdr (car (car x)))
   (cdadr x)       (cdr (car (cdr x)))
   (cddar x)       (cdr (cdr (car x)))
   (cdddr x)       (cdr (cdr (cdr x)))
   (caaaar x)      (car (car (car (car x))))
   (caaadr x)      (car (car (car (cdr x))))
   (caadar x)      (car (car (cdr (car x))))
   (caaddr x)      (car (car (cdr (cdr x))))
   (cadaar x)      (car (cdr (car (car x))))
   (cadadr x)      (car (cdr (car (cdr x))))
   (caddar x)      (car (cdr (cdr (car x))))
   (cadddr x)      (car (cdr (cdr (cdr x))))
   (cdaaar x)      (cdr (car (car (car x))))
   (cdaadr x)      (cdr (car (car (cdr x))))
   (cdadar x)      (cdr (car (cdr (car x))))
   (cdaddr x)      (cdr (car (cdr (cdr x))))
   (cddaar x)      (cdr (cdr (car (car x))))
   (cddadr x)      (cdr (cdr (car (cdr x))))
   (cdddar x)      (cdr (cdr (cdr (car x))))
   (cddddr x)      (cdr (cdr (cdr (cdr x))))
```
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
```scheme
   (range n) -> list?
   n : integer?
```
```scheme
   (print elt ...) -> void?
   elt : any/c
```
