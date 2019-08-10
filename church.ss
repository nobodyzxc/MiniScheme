; church number in lambda calculas

(define zero
  (lambda (f x) x))

(define succ
  (lambda (n)
    (lambda (f x) (f (n f x)))))

(define cnum
  (lambda (n)
    (if (= n 0)
        zero
        (succ (cnum (- n 1))))))

(define add
  (lambda (n m)
    (lambda (f x) (n f (m f x)))))

(define nnum
  (lambda (n)
    (n (lambda (e) (+ e 1)) 0)))

(define true
  (lambda (t f) t))

(define false
  (lambda (t f) f))


(define (fuck)
  (if #t 'a 'b))

(define if
  (lambda
    (bool then els)
    (bool then els)))

(define not
  (lambda (b)
    (b false true)))

(define and
  (lambda (n m)
    (n (m true false) false)))

(define or
  (lambda (n m)
    (n true (m true false))))

