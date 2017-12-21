(define f (lambda (i acc) (if (= i 0) acc (f (- i 1) (+ acc i)))))
;(display (f 10 0))
;(f 10 1) ; segmentation fault , tlc-opt
(display (f 10000 1)) ; segmentation fault , tlc-opt
;(f 49000 1) ; segmentation fault , tlc-opt
