(define fact-tail
    (lambda (x k)
        (if (= x 1)
            (k 1)
            (fact-tail (- x 1)
                (lambda (y) (k (* y x)))))))
(fact-tail 10 display)
