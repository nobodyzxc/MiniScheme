#lang r5rs
(define-syntax r
  (syntax-rules ()
    ((_) #t)
    ((_ e) e)
    ((_ e1 e2 ...)
     (if e1
	 (r e2 ...)
	 #f)))
  )

(r 1 2)
