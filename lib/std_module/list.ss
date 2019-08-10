; classic define
(define (caar x)   (car (car x)))
(define (cadr x)   (car (cdr x)))
(define (cdar x)   (cdr (car x)))
(define (cddr x)   (cdr (cdr x)))
(define (caaar x)  (car (car (car x))))
(define (caadr x)  (car (car (cdr x))))
(define (cadar x)  (car (cdr (car x))))
(define (caddr x)  (car (cdr (cdr x))))
(define (cdaar x)  (cdr (car (car x))))
(define (cdadr x)  (cdr (car (cdr x))))
(define (cddar x)  (cdr (cdr (car x))))
(define (cdddr x)  (cdr (cdr (cdr x))))
(define (caaaar x) (car (car (car (car x)))))
(define (caaadr x) (car (car (car (cdr x)))))
(define (caadar x) (car (car (cdr (car x)))))
(define (caaddr x) (car (car (cdr (cdr x)))))
(define (cadaar x) (car (cdr (car (car x)))))
(define (cadadr x) (car (cdr (car (cdr x)))))
(define (caddar x) (car (cdr (cdr (car x)))))
(define (cadddr x) (car (cdr (cdr (cdr x)))))
(define (cdaaar x) (cdr (car (car (car x)))))
(define (cdaadr x) (cdr (car (car (cdr x)))))
(define (cdadar x) (cdr (car (cdr (car x)))))
(define (cdaddr x) (cdr (car (cdr (cdr x)))))
(define (cddaar x) (cdr (cdr (car (car x)))))
(define (cddadr x) (cdr (cdr (car (cdr x)))))
(define (cdddar x) (cdr (cdr (cdr (car x)))))
(define (cddddr x) (cdr (cdr (cdr (cdr x)))))

(define (any lst)
  (if (null? lst) #f
    (if (car lst) #t
      (any (cdr lst)))))

(define (find pred lst)
  (if (null? lst) #f
    (if (pred (car lst))
      (car lst)
      (find pred (cdr lst)))))

(define (all lst)
  (if (null? rest) #t
    (if (car rest)
      (all (cdr lst)) #f)))

(define list
  (lambda lst lst))

(define (append lst . rest)
  (define (append1 a b)
    (if (null? a) b
      (cons (car a) (append1 (cdr a) b))))
  (if (null? rest) lst
    (apply append
           (cons (append1 lst (car rest)) (cdr rest)))))

(define (assoc elt ls)
  (if (null? ls) #f
    (if (equal? elt (caar ls)) (car ls) (assoc elt (cdr ls)))))

(define (foldl proc init lst . rest)
  (define (fold1 f e l)
    (if (null? l) e (fold1 f (f (car l) e) (cdr l))))
  (fold1 proc init (apply append (cons lst rest))))

(define (foldr proc init lst . rest)
  (define (fold1 f e l)
    (if (null? l) e (f (car l) (fold1 f e (cdr l)))))
  (fold1 proc init (apply append (cons lst rest))))

(define fold foldl)

(define (reverse xs)
  (foldl cons '() xs))

(define (foldr proc init lst . rest)
  (foldl proc init (reverse (apply append (cons lst rest)))))

(define (filter f l)
  (if (null? l) '()
    (if (f (car l))
      (cons (car l) (filter f (cdr l)))
      (filter f (cdr l)))))

; depend on foldl

(define (map func lst . rest)
  (define (map1 func lst)
    (reverse
      (foldl
        (lambda (v acc) (cons (apply func v) acc))
        '()
        lst)))
  (define (zip . rest)
    (if (or (null? rest) (null? (car rest))) '()
      (cons (foldr (lambda (v acc) (cons (car v) acc)) '()  rest)
            (apply zip (foldr (lambda (v acc) (cons (cdr v) acc)) '() rest)))
      )
    )
  (map1 func (apply zip (cons lst rest))))
