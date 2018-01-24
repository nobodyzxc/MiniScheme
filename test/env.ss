(define (f x)
  (print "f-env\n" (get-curenv))
  (g (+ x 1)))

(define (g y)
  (print "g-env\n" (get-curenv)))

(f 4)
