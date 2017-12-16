(define f (lambda (i acc) (if (= i 0) acc (f (- i 1) (+ acc i)))))
(f 10 0)
;(f 10 1) ; segmentation fault , tlc-opt
;(f 10000 1) ; segmentation fault , tlc-opt
;(f 49000 1) ; segmentation fault , tlc-opt
