(define (list . args)
  (if (null? args) '()
    (cons (car args) (apply list (cdr args)))))

(define (append a b)
  (if (null? a) b
    (cons (car a) (append (cdr a) b))))

(define (map1 f l)
  (if (null? l) '()
    (cons (f (car l)) (map1 f (cdr l)))))

(define (map func . ls)
  (if (null? (car ls)) '()
    (cons (apply func (map1 car ls))
          (apply map (cons func (map1 cdr ls))))))

(define (fold f b l)
  (if (null? l) b (fold f (f (car l) b) (cdr l))))

(define (filter f l)
  (if (null? l) '()
    (if (f (car l))
      (cons (car l) (filter f (cdr l)))
      (filter f (cdr l)))))

(define (caar l) (car (car l)))
(define (cadr l) (car (cdr l)))
(define (caddr l) (car (cdr (cdr l))))
(define (cadddr l) (car (cdr (cdr (cdr l)))))
(define (tab) (senv global))
(define (newline) (display "\n"))

(define (assoc elt ls)
  (if (null? ls) #f
    (if (= elt (caar ls)) (car ls) (assoc elt (cdr ls)))))

(define (index-of-iter cmp elt ls cur)
  (if (null? ls) -1
    (if (cmp (car ls) elt) cur
      (index-of-iter cmp elt (cdr ls) (+ cur 1)))
    ))

(define (index-of cmp elt ls)
  (index-of-iter cmp elt ls 0))

(define indexof
  (syntax-rules
    (in with)
    ((_ elt in ls with cmp) (indexof cmp elt ls)))
  )

(define or
  (syntax-rules
    ()
    ((_) #t)
    ((_ exp) (if exp #t #f))
    ((_ exp rest ...) (if exp #t (or rest ...))))
  )

(define and
  (syntax-rules
    ()
    ((_) #f)
    ((_ exp) (if exp #t #f))
    ((_ exp rest ...) (if exp (and rest ...) #f)))
  )

(define let
  (syntax-rules
    ()
    ((_ ((bind args) ...) body ...)
     ((lambda (bind ...) body ...) args ...)))
  )

(define begin
  (syntax-rules
    ()
    ((_ expr ...)
     ((lambda () expr ...))))
  )

(define cond
  (syntax-rules
    (else)
    ((_) (void))
    ((_ (else expr) rest ...)
     (if #t expr))
    ((_ (pred expr) rest ...)
     (if pred expr (cond rest ...))))
  )

(define case
  (syntax-rules
    (else)
    ((_ pred) (void))
    ((_ want (else expr) rest ...)
     (if #t expr))
    ((_ want (pred expr) rest ...)
     (if (>= (index-of equal? want 'pred) 0)
       expr (case want rest ...)))
    )
  )
