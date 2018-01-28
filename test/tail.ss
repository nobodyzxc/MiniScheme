; will failed if disable TCO
(define (f acc e)
  (if (= e 0) acc (f (+ e acc) (- e 1))))
(print (f 0 19999))
