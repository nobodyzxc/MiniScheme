(define (map1 f l)
  (if (null? l) '()
    (cons (f (car l)) (map1 f (cdr l)))))

(define (map func . ls)
  (if (null? (car ls)) '()
    (cons (apply func (map1 car ls))
          (apply map (cons func (map1 cdr ls))))))

(define (fold f b l)
  (if (null? l) b (fold f (f (car l) b) (cdr l))))

(define (filter f l)
  (if (null? l) '()
    (if (f (car l))
      (cons (car l) (filter f (cdr l)))
      (filter f (cdr l)))))
