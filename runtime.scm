(define length (lambda (x)
	(if (null? x) 0 (+ 1 (length (cdr x))))
))

(define list (lambda x
	(if (null? x)
		nil
		(cons (car x) (list . (cdr x))
	)
))