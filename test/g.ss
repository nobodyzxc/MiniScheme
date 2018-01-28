(define h
  (lambda (x)
    (if (= x 0)
      0
      (+ x (h (- x 1)))
      )
    )
  )

(define h
  (lambda (x k)
    (if (= x 0)
      (k 0)
      (h (- x 1)
         (lambda (v)
           (k (+ x v))))
      )
    )
  )

(h 10 print)
