(cond ((= 1 2) 1) (else 2))
(cond ((= 1 2) 1)) ; => void symbol , because biwa doesn't support (void)
(define a (lambda (x) (if (= x 0) 0 (+ x (a (- x 1))))))
(let* ((a 1) (b (+ 1 a))) (+ a b))
(let* ((a 1) (b (+ 1 a))) (let* ((c 5)) (+ a b c)))
(letrec ((a (lambda (x) (if (= x 0) 0 (+ x (a (- x 1))))))) (a 3))
(letrec ((f (lambda (x) (if ( = x 1) 1 (+ x (g (- x 1))))))
         (g (lambda (x) (if ( = x 1) 1 (+ x (f (- x 1)))))))
  (g 5))
(letrec ((f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1)))))))
  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
    (f (g 3))
    )
  )
(letrec ((f (lambda (x) (if (= x 0) 0 (+ x (f (- x 1)))))))
  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
    (f (g 3))
    )
  )
(letrec ((f 3))
  (letrec ((g (lambda (x) (if (= x 0) 0 (+ x (g (- x 1)))))))
    (g f)
    )
  )
