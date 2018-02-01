(define g (lambda (x acc)
            (if (= x 0) acc
              (g (- x 1) (+ acc x)))))

(display (g 10 0))
(display "\n")
