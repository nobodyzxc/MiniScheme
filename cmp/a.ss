(define (f acc it)
  (if (= it 0) acc (f (+ acc it) (- it 1))))
(display (f 0 10))
;(system "sleep 5")
