;(define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
;(f 100000) ; Can I opt it to tail call? (Do I need CPS?)
(define g (lambda (x acc)
            (if (= x 0) acc
              (g (- x 1) (+ acc x)))))

;(define f (lambda (x k)
;            (if (= x 0)
;              (k 0)
;              (f (- x 1) (lambda (v)
;                           (k (+ x v)))))))

(print (g 100000000 0))
;(f 100000000 print)
