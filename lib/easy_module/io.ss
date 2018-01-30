(define (print . args)
  (if (null? args) (newline)
    (begin
      (display (car args))
      (apply print (cdr args)))))

(define (show . args)
  (if (not (null? args))
    (begin
       (display (car args))
       (apply show (cdr args)))))
