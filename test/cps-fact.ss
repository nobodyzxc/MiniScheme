
(define (fact n k)
    (if (<= n 2)
        (k n)
        (fact (- n 1)
            (lambda (v)
                (k (* v n))))))
(fact 5 display)