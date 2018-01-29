(define (map func . ls)
  (define (map1 f l)
    (if (null? l) '()
      (cons (f (car l)) (map1 f (cdr l)))))
  (if (null? (car ls)) '()
    (cons (apply func (map1 car ls))
          (apply map (cons func (map1 cdr ls))))))

(define (fold f b . ls)
  (if (null? (car ls)) b
    (apply
      fold
      (cons f
            (cons
              (apply f (cons b (map car ls)))
              (map cdr ls))
            )
      )
    ))

(define (filter f l)
  (if (null? l) '()
    (if (f (car l))
      (cons (car l) (filter f (cdr l)))
      (filter f (cdr l)))))
