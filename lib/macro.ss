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
    (else =>)
    ((_) (void))
    ((_ (else expr ...) rest ...)
     (if #t (begin expr ...)))
    ((_ (pred => expr) rest ...)
     (if pred (expr pred) (cond rest ...)))
    ((_ (pred) rest ...)
     (if pred pred (cond rest ...)))
    ((_ (pred expr ...) rest ...)
     (if pred (begin expr ...) (cond rest ...)))
    ((_ expr rest ...)
     (if expr expr (cond rest ...))))
  )

(define case
  (syntax-rules
    (else)
    ((_ pred) (void))
    ((_ want (else expr ...) rest ...)
     (if #t (begin expr ...)))
    ((_ want (pred expr ...) rest ...)
     (if (>= (index-of equal? want 'pred) 0)
       (begin expr ...) (case want rest ...)))
    )
  )
