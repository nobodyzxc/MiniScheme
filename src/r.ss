(define *cont*
    (lambda (x) x))
(define (f a cont)
    (g 2
        (lambda (v)
            (cont (display a)))))

(define (g b cont)
    (cont (display b)))

(f 1 *cont*)
