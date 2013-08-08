#include "lisp.h"
#include "parser.h"
#include "listops.h"

int main(int argc, char **argv) {
    NODE *forms = parseForms("(+ 1 (* 2 6) (- 7 3))");
    int len = list_length(forms);
    printf("Evaluating %i forms\n",len);
    for (NODE *form = forms; form; form = (NODE*)form->addr) {
        VALUE *val = evaluate(form->data,NIL);
        printVal(val);
        decRef(val);
    }
    decRef(forms);
}
