(define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
(f 100)
(f 1000)
(f 3000)
; (f 4000) segmentation fault
