(define (index-of-iter cmp elt ls cur)
  (if (null? ls) -1
    (if (cmp (car ls) elt) cur
      (index-of-iter cmp elt (cdr ls) (+ cur 1)))
    ))

(define (range-it e acc)
  (if (< e 0) acc (range-it (- e 1) (cons e acc))))

(define (range x)
  (range-it x '()))


(define (index-of cmp elt ls)
  (index-of-iter cmp elt ls 0))

(define indexof
  (syntax-rules
    (in with)
    ((_ elt ls) (index-of equal? elt ls))
    ((_ elt in ls with cmp) (index-of cmp elt ls)))
  )

(define (even? x)
  (or (= 0 x) (= 1 (% 2 x))))
(define (odd? x) (not (even? x)))

(define (print . args)
  (map display args)(newline))

(define (show . args)
  (map display args))

