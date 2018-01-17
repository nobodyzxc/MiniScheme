(define cond
  (syntax-rules
    (else)
    ((_) (void))
    ((_ (else expr ...) rest ...)
     (if #t (begin expr ...)))
    ((_ (pred expr ...) rest ...)
     (if pred (begin expr ...) (cond rest ...))))
  )
(define (f acc it) (cond ((= it 0) acc) (else (f (+ acc it) (- it 1)))))
(display "apply cond-rec : ")(flush-output)
(display (f 0 5000))(newline)


