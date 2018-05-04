(define (range x)
  (define
    (range-it e acc)
    (if (< e 0)
      acc
      (range-it
        (- e 1)
        (cons e acc))))
  (range-it (- x 1) '()))


(define (index elt ls)
  (define (index-iter elt ls cur)
    (if (null? ls) -1
      (if (equal? (car ls) elt) cur
        (index-iter elt (cdr ls) (+ cur 1)))))
  (index-iter elt ls 0))

(define (snoc x lst)
  (append lst (list x)))
