;(define fact
;  (lambda (x)
;    (if (= x 1)
;      1
;      (* x (fact (- x 1))))))
;
;(define fact-tail
;  (lambda (x k)
;    (if (= x 1)
;      (k 1)
;      (fact-tail (- x 1)
;                 (lambda (y) (k (* y x)))))))
;
;(define fib
;  (lambda (n)
;    (if (<= n 2)
;      1
;      (+ (fib (- n 1))
;         (fib (- n 2))))))

;(define fib-tail
;  (lambda (n k)
;    (if (<= n 2)
;      (k 1)
;      (fib-tail (- n 1)
;                (lambda (x)
;                  (fib-tail (- n 2)
;                            (lambda (y)
;                              (k (+ x y)))))))))
(define fib-tail
  (lambda (n k)
    (if (<= n 2)
      (k 1)
      (fib-tail (- n 2)
                (lambda (x)
                  (fib-tail (- n 1)
                            (lambda (y)
                              (k (+ x y)))))))))

(fib-tail  10 print)
