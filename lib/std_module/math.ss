(define (succ v) (+ 1 v))

(define (even? x)
  (or (= 0 x) (= 1 (% 2 x))))

(define (odd? x) (not (even? x)))

