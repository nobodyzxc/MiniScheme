(print "OPT with CPS")
(define g (lambda (acc x) (if (= x 0) acc (g (+ acc x) (- x 1)))))
(display "tail call : ")
(display (g 0 10000))(newline)
(define f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1))))))
(display "normal call : ")(flush-output)
#| ; wait tail call opt
(display (f 10000))
(define g (lambda (acc x) (+ 1 1 acc x)))
|#
