(define (list . args)
    (if (null? args) '()
      (cons (car args) (apply list (cdr args)))))

;(define (map func . ls)
;  (if (null? ls) '()
;    (cons (apply func (car ls)) (map func (cdr ls)))))

(define (tab) (senv global))
