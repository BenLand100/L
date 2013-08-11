(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) 
(macro defun (symbol args &rest body) (list 'bind (list 'quote symbol) (node 'lambda (node args body))))
(macro if (test-case true-form false-form) (list 'cond (list test-case true-form) (list 't false-form)))
(bind 't 't)

(defun add (x y) (+ x y) ( + x y x y))

(if nil (+ 1 2) (+ 2 4))
