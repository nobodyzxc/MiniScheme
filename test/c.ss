(display "> test 2 : macro speed\n")
(define (f acc it) (cond ((= it 0) acc) (else (f (+ acc it) (- it 1)))))
(define (g acc it) (if (= it 0) acc (g (+ acc it) (- it 1))))
(define arg 10000)
(print "apply cond-rec : ")
(print "(define (g acc it)\n"
       "  (if (= it 0)\n"
       "     acc\n"
       "     (g (+ acc it) (- it 1)))")
(show "(f 0 " arg ")")
(flush-output)
(print " = " (f 0 arg))
