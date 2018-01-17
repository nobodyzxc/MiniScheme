(define (g acc it) (if (= it 0) acc (g (+ acc it) (- it 1))))
(display "apply if-rec : ")
(display (g 0 5000))(newline)
