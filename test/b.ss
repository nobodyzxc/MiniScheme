(display "> test 2 : macro speed\n")
(define (f acc it) (cond ((= it 0) acc) (else (f (+ acc it) (- it 1)))))
(define (g acc it) (if (= it 0) acc (g (+ acc it) (- it 1))))
(define arg 10000)
(print "apply if-rec : ")
(print "(define (f acc it)\n"
       "  (cond\n"
       "    ((= it 0) acc)\n"
       "    (else (f (+ acc it) (- it 1)))))")
(print "(g 0 " arg ")" " = " (g 0 arg))


