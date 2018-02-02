;;; Scheme in Scheme, Mini-Scheme Version 2
;;; enter (exit) to quit
(define (scheme) ;;; the read-eval-print loop.
  (display "myScheme> ")
  (let ((expr (read)))
    (cond ((equal? expr '(exit))      ; (exit) only allowed at top level
           'done)
          (else  (display (top-eval expr))
                 (newline)
                 (scheme))
          )))

;;; not needed for assignment
(define (my-load filename)       ;; don't want to redefine the Scheme LOAD
  (load-repl (open-input-file filename)))

(define (load-repl port)  ;read-eval-print-loop: repl
  (let ((expr (read port)))
    (cond ((eof-object? expr) 'done)
          (else (let ((res (top-eval expr)))
                  (display res) (newline)
                  (load-repl port)))
          )))

;; (define ....) is only allowed at the top level and affects only the
;; global environment. Only the basic form of define is supported here.

(define (top-eval expr)
  (cond ((not (pair? expr)) (my-eval expr globals))
        ((eq? (car expr) 'define)         ;LOOK CAREFULLY HERE.
         (set! globals (my-define (cdr expr) globals))) ;assignment
        ((eq? (car expr) 'showenv) globals)
        (else (my-eval expr globals))
        ))


;;; Representation of non-primitive function: (%func args body env)
(define make-func
  (lambda (var-list body env)
    (list '%func var-list body env)))


;; display error message
(define (error msg s) (display msg))

;; Global environment
(define globals
  (list
    (list '+ +)
    (list '- -)
    (list '* *)
    (list '/ /)
    (list '= =)
    (list '> >)
    (list 'fact (make-func '(n) '(if (= n 0) 1 (* n (fact (+ n -1)))) '()) )
    (list 'car car)
    (list 'cdr)
    (list 'null? null?)
    (list 'cons cons)
    (list 'eq? eq?)
    (list 'pair? pair?)
    (list 'not (make-func '(x) '(eq? x #f) '()) )
    (list 'equal? (make-func '(a b)
                             '(if (pair? a)
                                (if (pair? b)
                                  (if (equal? (car a) (car b))
                                    (equal? (cdr a) (cdr b))
                                    #f)
                                  #f)
                                (if (pair? b)
                                  #f
                                  (eq? a b)))
                             '()))
    ))

;;; Representation of environment: association list.
;; ((name_1 value_1) ... (name_n value_n))

;;; To look up a variable, first look in "local" environment, then in
;;; global invironment.
(define lookup-symbol
  (lambda (s env)
    (cond ((assoc s env) => cadr)
          ((assoc s globals) => cadr)
          (else (error "symbol not found:" s)))))



;;Helper fun for constructing environments
(define (zip z lst1 lst2)
  (if (null? lst1)
    '()
    (cons
      (z (car lst1) (car lst2))
      (zip z (cdr lst1) (cdr lst2)))))

;; Predicates for checking forms of Expressions
(define selfevaluating?
  (lambda (expr)
    (not (pair? expr))))

(define lambda?
  (lambda (expr)
    (eq? (car expr) 'lambda)))

(define quote?
  (lambda (expr)
    (eq? (car expr) 'quote)))

(define if?
  (lambda (expr)
    (eq? (car expr) 'if)))

(define cond?
  (lambda (expr)
    (eq? (car expr) 'cond)))

(define let?
  (lambda (expr)
    (eq? (car expr) 'let)))

(define let*?
  (lambda (expr)
    (eq? (car expr) 'let*)))

(define letrec?
  (lambda (expr)
    (eq? (car expr) 'letrec)))

;;main evaluating function

(define my-eval
  (lambda (expr env)
    (cond ((symbol? expr)	;normal variable
           (lookup-symbol expr env))
          ((selfevaluating? expr) expr)	;self evaluating
          ;;Special forms: LAMBDA, QUOTE, IF
          ((lambda? expr)	;LAMBDA
           (make-func (cadr expr) (caddr expr) env))
          ((quote? expr)	;QUOTE
           (cadr expr))
          ((if? expr)		;IF
           (my-eval ((if (my-eval (cadr expr) env)
                       caddr
                       cadddr)
                     expr)
                    env))

          ((cond? expr)          ;COND
           (my-cond (cdr expr) env)
           )

          ((let? expr)	;LET
           (let* ( (clauses (cadr expr))
                  (body (caddr expr))
                  (vars (map car clauses))
                  (exprs (map cadr clauses))
                  (vals (map (lambda (e) (my-eval e env)) exprs))
                  (new-env (append (zip list vars vals) env)) )
             (my-eval body new-env)))

          ((let*? expr)  ; LET*
           (my-let* (cdr expr) env))

          ((letrec? expr)  ; LETREC
           (my-letrec (cadr expr) (caddr expr) env)
           )

          (else   ;regular funcall
            (let ( (eexpr (map (lambda (e) (my-eval e env))
                               expr)))
              (my-apply (car eexpr) (cdr eexpr)))))))

;; extend an environment
(define (extend-env vars vals env)
  (append (zip list vars vals) env))

;; Apply a function to its arguments
(define my-apply
  (lambda (f args)
    (cond ((procedure? f)		; primitive function
           (apply f args))
          ((and (pair? f) (eq? (car f) '%func)) ; non-primitive function
           (my-eval (caddr f) ; the proc body
                    (extend-env (cadr f) ;parameters
                                args
                                (cadddr f)) ;env-in-closure
                    ))
          (else (error "invalid function in MY-APPLY -> " f)))))

(define (my-cond expr env)
  (if (null? expr) 'void
    (let ((boolexp (caar expr))
          (execexp (cadar expr)))
      (if (eq? boolexp 'else)
        (my-eval execexp env)
        (if (eq? #t (my-eval boolexp env))
          (my-eval execexp env)
          (my-cond (cdr expr) env)
          )
        )
      )
    )
  )

(define (my-define expr env)
  (let ((fh (car expr)) (fb (cadr expr)))
    (if (pair? fh)
      (cons (list (car fh) (make-func (cdr fh) fb '())) env)
      (cons (list fh (my-eval fb env)) env)
      ;(cons (list fh (make-func '() fb '())) env)
      )
    )
  )

;(list 'fact (make-func '(n) '(if (= n 0) 1 (* n (fact (+ n -1)))) '()) )

(define (my-let* expr env)
  (let ((defs (car expr)) (exp-body (cadr expr)))
    (if (null? defs)
      (my-eval exp-body env)
      (let ((var (caar defs))
            (val (cadar defs))
            (left-defs (cdr defs))
            )
        (my-let* (list left-defs exp-body)
                 (cons (list var (my-eval val env)) env)
                 )
        )
      )
    )
  )

(define (init-env defs)
  (map (lambda (e) (list (car e) 'undef)) defs)
  )

(define (rec-env defs env)
  (let ()
    (map (lambda (e) (set-cdr! (assoc (car e) env) (list (my-eval (cadr e) env)))) defs)
    env
    )
  )
; if use set! will be inf rec , don't know why ... wait to find out

(define (my-letrec defs body env)
  (my-eval body (rec-env defs (append (init-env defs) env))))
; append avoid set-cdr modify the globals?
;; Starting the mini-scheme interpreter
(scheme)

; tests
; teacher's let => (let () 1 2 3) => 1 , but in normal interp should return 3
; 'cause it take caddr while not cddr to interp recursively

;#|
;(cond ((= 1 2) 1) (else 2))
;(cond ((= 1 2) 1)) ; => void symbol , because biwa doesn't support (void)
;(define a (lambda (x) (if (= x 0) 0 (+ x (a (- x 1))))))
;(let* ((a 1) (b (+ 1 a))) (+ a b))
;(let* ((a 1) (b (+ 1 a))) (let* ((c 5)) (+ a b c)))
;(letrec ((a (lambda (x) (if (= x 0) 0 (+ x (a (- x 1))))))) (a 3))
;(letrec ((f (lambda (x) (if ( = x 1) 1 (+ x (g (- x 1))))))
;         (g (lambda (x) (if ( = x 1) 1 (+ x (f (- x 1)))))))
;  (g 5))
;(letrec ((f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1)))))))
;  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
;    (f (g 3))
;    )
;  )
;(letrec ((f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1)))))))
;  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
;    (f (g 3))
;    )
;  )
;(letrec ((f 3))
;  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
;    (g f)
;    )
;  )
;|#
