(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) 
(macro defun (symbol args &rest body) (list 'bind (list 'quote symbol) (node 'lambda (node args body))))
(macro if (test-case true-form &optional false-form) (node 'cond (node (list test-case true-form) (cond (false-form (node (list 't false-form)  NIL))))))
(bind 't 't)

(defun add (x y) (+ x y) ( + x y x y))

(if t (+ 1 2))
