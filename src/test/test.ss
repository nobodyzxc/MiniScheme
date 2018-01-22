(display "> test 1 HOF :\n")
(define (range-it e acc)
  (if (< e 0) acc (range-it (- e 1) (cons e acc))))

(define (range x)
  (range-it x '()))

(define (fold f b l)
  (if (null? l) b (fold f (f (car l) b) (cdr l))))

(print "(display (fold + 0 (range 1000)))")
(print "= " (fold + 0 (range 1000)))

(newline)

(display "> test 2 : macro speed\n")
(define (f acc it) (cond ((= it 0) acc) (else (f (+ acc it) (- it 1)))))
(define (g acc it) (if (= it 0) acc (g (+ acc it) (- it 1))))
(print "apply if-rec : ")
(print "(define (f acc it)\n"
       "  (cond\n"
       "    ((= it 0) acc)\n"
       "    (else (f (+ acc it) (- it 1)))))")
(print "= " (g 0 10000))
(newline)
(print "apply cond-rec : ")
(print "(define (g acc it)\n"
       "  (if (= it 0)\n"
       "     acc\n"
       "     (g (+ acc it) (- it 1)))")
(flush-output)
(print "= " (f 0 10000))

(newline)

(display "> test 3 : TCO stack test\n")
(define (tail acc it)
  (if (= it 0) acc (tail (+ acc it) (- it 1))))
(print "(define (tail acc it)\n
  (if (= it 0) acc (tail (+ acc it) (- it 1))))")
(print "(tail 0 10000)")
(print "= " (tail 0 100000))

(newline)

(display "> test 4 : macro in macro\n")
(print
  "(begin (cond (#f 1) (else 2))) = "
  (begin (cond (#f 1) (else 2))))

(newline)

(display "> test 5 : define with head form\n")
(define ((f a) b) (+ a b))
(print "(define ((f a) b) (+ a b))")
(define fa (f 1))
(print "(define fa (f 1))")
(print "(fa 1) = " (fa 1))
(define fb (f 4))
(print "(define fb (f 4))")
(print "(fb 1) = " (fb 1))

(newline)

(display "> test 6 : set-cdr! set-car!\n")
(define y (list 1 2 3 4))
(define x 3)
(print "y -> " y)
(print "x -> " x)
(print "(set-car! y x)")
(set-car! y x)
(print "y -> " y)
(print "(set-cdr! y x)")
(set-cdr! y x)
(print "y -> " y)

(newline)

(display "> test 7 : TCO env test\n")
(define *cont*
    (lambda (x) x))

(define (f a cont)
    (g 2 (lambda (v)
            (cont (print a)))))

(define (g b cont)
    (cont (print b)))

(f 1 *cont*)

(newline)
