#lang r5rs

(define (void? e)
  (eq? e (display "")))

(define print
  (lambda args
    (begin
      (apply display args)
      (flush-output))
    )
  )

(define println
  (lambda args
    (begin
      (apply display args)
      (newline)(flush-output))
    )
  )

(define error
  (lambda args
    (if (not (null? args))
      (begin
        (display (car args))
        (apply error (cdr args))
        ))))

(define errorln
  (lambda args
    (if (null? args)
      (begin
        (newline)(flush-output))
      (begin
        (display (car args))
        (apply error (cdr args))
        ))))

(define (extend-env names binds env)
  (if (null? names) env
    (extend-env (cdr names)
                (cdr binds)
                (cons
                  (cons (car names)
                        (car binds))
                  env)
                )
    ))

(define (exit) 'exit)
(define glenv
  (list
    (cons 'display  display)
    (cons 'list     list)
    (cons 'exit     exit)
    (cons 'cons     cons)
    (cons 'car      car)
    (cons '+        +)
    (cons '-        -)
    (cons '*        *)
    (cons '/        /)
    (cons '=        =)
    )
  )

(define (mini-defenv expr env)
  (let ((fh (car expr)) (fb (cadr expr)))
    (if (pair? fh)
      (cons (cons (car fh) (make-func (cdr fh) fb '())) env)
      (cons (cons fh (mini-eval fb env)) env)
      )
    )
  )

(define (repl)
  (print "MiniEval> ")
  (let ((expr (read)))
    (cond
      ((and (pair? expr)
            (not (null? expr))
            (eq? (car expr) 'exit))
       'exit)
      ((and (pair? expr)
            (not (null? expr))
            (eq? (car expr) 'define))
       (begin
         (set! glenv (mini-defenv (cdr expr) glenv))
         (repl)))
      (else
        (let ((v (mini-eval expr glenv)))
          (if (not (void? v)) (print v))
          (println "") (repl)))
      )
    )
  )

(define lookup-symbol
  (lambda (s env)
    (cond ((assoc s env) => cdr)
          ((assoc s glenv) => cdr)
          (else (error "unbound symbol: " s)))))

(define (self-eval? expr)
  (not (list? expr)))

(define (lambda? expr)
  (eq? (car expr) 'lambda))

(define (quote? expr)
  (eq? (car expr) 'quote))

(define (if? expr)
  (eq? (car expr) 'if))

(define (mini-if expr env)
  (mini-eval
    ((if (mini-eval
           (cadr expr) env)
       caddr
       cadddr)
     expr) env))

(define mini-apply
  (lambda (func args)
    (cond ((procedure? func) ;if build-in func
           (apply func args))
          ((and (list? func) ;if lambda
                (eq? (car func) 'mini-lambda))
           (mini-eval (caddr func)
                      (extend-env
                        (cadr func) args ;new bindings
                        (cadddr func)))) ;original env
          ((void? func) "")
          (else (error "cannot apply: " func)))))

(define (mini-eval expr env)
  (cond ((symbol? expr)
         (lookup-symbol expr env))
        ((self-eval? expr)
         expr)
        ((lambda? expr)
         (make-func (cadr expr) (caddr expr) env))
        ((quote? expr)
         (cadr expr))
        ((if? expr)
         (mini-if expr env))
        ;((cond? expr)
        ; 'not-ready-yet)
        ;((let? expr)
        ; 'not-ready-yet)
        ;((let*? expr)
        ; 'not-ready-yet)
        ;((letrec? expr)
        ; 'not-ready-yet)
        (else ;regular func call
          (let ((evaled-expr
                  (map (lambda (e)
                         (mini-eval e env)) expr)))
            (mini-apply (car evaled-expr)
                        (cdr evaled-expr)))
          )
        ))

(define make-func
  (lambda (args body env)
      (list 'mini-lambda args body env)))

(repl)
