; will failed if disable TCO
(define y 1)

(define (fun acc a b c d e f g h i j k l m n)
  (if (= a 0)
    acc
    (fun (+ acc y) (- a 1) b c d e f g h i j k l m n)
    )
  )
(print (fun 0 19999 1 1 1 1 1 1 1 1 1 1 1 1 1))
#|
(define (fun acc a)
  (if (= a 0)
    acc
    (fun (+ acc y) (- a 1))
    )
  )
(print (fun 0 19999))
|#

