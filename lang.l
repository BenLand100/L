;language macros
(macro set (symbol value) (list 'seta (list 'ref (list 'quote symbol)) value)) 
(macro defun (symbol args &rest body) (list 'bind (list 'quote symbol) (node 'lambda (node args body))))
(macro call (func &rest args) (node func args))
(macro if (test-case true-form &optional false-form) (node 'cond (node (list test-case true-form) (cond (false-form (node (list 't false-form)  NIL))))))
(macro let (variables &rest forms) 
    (bind 'map (lambda (func args-list) (if args-list (node (func (data args-list)) (map func (addr args-list))))))
    (node (node 'lambda (node (map (lambda (variable) (data variable)) variables) forms)) (map (lambda (variable) (data (addr variable))) variables)) )

;language symbols
(bind 't 't)

;languge functions
(defun map (func args-list) (if args-list (node (func (data args-list)) (map func (addr args-list)))))

;test
(print 'welcome 'to 'l)
(print '(+ 1 1) '= (+ 1 1))

(defun inc (x) (+ 1 x))
(defun sqr (x) (* x x))

(bind 'vals '(1 2 3 4))
(print 'vals vals)
(print 'inc (map inc vals))
(print 'sqr (map sqr vals))

(print 'let 'test (let ((a (lambda (x y) (+ x y))) (b 2) (c 3)) (a b c)) )
