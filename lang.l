(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) 
(macro defun (symbol args body) (list 'bind (list 'quote symbol) (list 'lambda args body)))

(defun add (x y) (+ x y))
(add 1 2)
