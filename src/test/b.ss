(define (f acc it) (cond ((= it 0) acc) (else (f (+ acc it) (- it 1)))))
(display "apply cond-rec : ")(flush-output)
(display (f 0 5000))(newline)

