(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) 
(macro defun (symbol args &rest body) (list 'bind (list 'quote symbol) (node 'lambda (node args body))))
(macro if (test-case true-form false-from) (list 'cond (list test-case true-form) (list t false-form)))

(defun add (x y) (+ x y) ( + x y x y))
(add 1 2)
