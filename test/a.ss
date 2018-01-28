(define pb 1000)
(print "OPT with CPS")
(define f
  (lambda (acc x)
    (if (= x 0)
      acc
      (f (+ acc x) (- x 1)))
    )
  )

(display "classic tail call : ")
(display (f 0 pb))(newline)

(define g
  (lambda (x k)
    (if (= x 0)
      (k x)
      (g (- x 1) (lambda (v) (k (+ x v)))))
    )
  )
(display "cps tail call : ")
(g pb display)(newline)

(define h (lambda (x) (if (= x 0) 0 (+ x (h (- x 1))))))
(display "normal call : ")(flush-output)
; wait tail call opt
(display (h 10000))
