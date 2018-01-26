(define (range x)
  (define
    (range-it e acc)
    (if (< e 0)
      acc
      (range-it
        (- e 1)
        (cons e acc))))
  (range-it x '()))


(define (indexof elt ls)
  (define (indexof-iter elt ls cur)
    (if (null? ls) -1
      (if (equal? (car ls) elt) cur
        (indexof-iter elt (cdr ls) (+ cur 1)))))
  (indexof-iter elt ls 0))

(define (even? x)
  (or (= 0 x) (= 1 (% 2 x))))
(define (odd? x) (not (even? x)))

(define (print . args)
  (map display args)(newline))

(define (show . args)
  (map display args))

