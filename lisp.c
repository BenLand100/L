#include "lisp.h"
#include "scope.h"
#include "primitives.h"

void freeVALUE(VALUE *val) {
    switch (val->type) {
        case ID_NODE:
            decRef(((NODE*)val)->data);
            decRef(((NODE*)val)->addr);
            break;
        case ID_STRING:
            free(((STRING*)val)->str);
            break;
    }
    free(val);
}

VALUE* deep_copy(VALUE *val) {
    if (!val) return NIL;
    switch (val->type) {
        case ID_NODE:
            return (VALUE*)newNODE(deep_copy(((NODE*)val)->data),deep_copy(((NODE*)val)->addr));
        case ID_SYMBOL:
            return (VALUE*)newSYMBOL(((SYMBOL*)val)->sym);
        case ID_INTEGER:
            return (VALUE*)newINTEGER(((INTEGER*)val)->val);
        case ID_REAL:
            return (VALUE*)newREAL(((REAL*)val)->val);
        case ID_STRING:
            return (VALUE*)newSTRING(strdup(((STRING*)val)->str));
        case ID_PRIMFUNC:
            return (VALUE*)newPRIMFUNC(((PRIMFUNC*)val)->id);
    }
}

void printList(NODE *list) {
    if (list->addr) {
        if (list->addr->type == ID_NODE) {
            print(list->data);
            printList((NODE*)list->addr);
        } else {
            print(asVALUE(list));
        }
    } else {    
        print(list->data);
    }
}

void print(VALUE *val) {
    if (!val) {
        printf("NIL ");
        return;
    }
    switch (val->type) {
        case ID_NODE:
            printf("( ");
            if (((NODE*)val)->addr && ((NODE*)val)->addr->type != ID_NODE) {
                print(((NODE*)val)->data);
                printf(". ");
                print(((NODE*)val)->addr);
            } else {
                printList(((NODE*)val));
            }
            printf(") ");
            return;
        case ID_INTEGER:
            printf("%i ", ((INTEGER*)val)->val);
            return;
        case ID_REAL:
            printf("%f ", ((REAL*)val)->val);
            return;
        case ID_SYMBOL:
            printf("SYMBOL_%i ",((SYMBOL*)val)->sym);
            return;
        case ID_STRING:
            printf("\"%s\"",((STRING*)val)->str);
            return;
        case ID_PRIMFUNC:
            printf("PRIMFUNC_%i ",((PRIMFUNC*)val)->id);
            return;
    }
}

#define prim_func(name) { \
    NODE *args_eval = list(args,scope); \
    VALUE *res = (VALUE*)name(args_eval,scope); \
    decRef(args_eval); \
    return res; \
}

#define prim_macro(name) return (VALUE*)name(args,scope)

VALUE* funcall(VALUE *func, NODE *args, NODE *scope) {
    debugVal(func,"Funcall: ");
    failNIL(func,"NIL cannot be invoked");
    switch (func->type) {
        case ID_PRIMFUNC:
            switch (((PRIMFUNC*)func)->id) {
                case PRIM_LIST: prim_macro(list);
                case PRIM_QUOTE: prim_macro(quote);
                case PRIM_ADDR: prim_func(addr);
                case PRIM_DATA: prim_func(data);
                case PRIM_ADD: prim_func(add);
                case PRIM_SUB: prim_func(sub);
                case PRIM_MUL: prim_func(mul);
                case PRIM_DIV: prim_func(div);
                default: error("Unhandled PRIMFUNC");
            }
        case ID_NODE: {
            NODE *vars = asNODE(((NODE*)func)->data);
            NODE *args_eval = list(args,scope);
            scope = pushScope(scope);
            bindMany(vars,args_eval,scope);
            decRef(args_eval);
            VALUE *res = evaluate(((NODE*)func)->addr,scope);
            scope = popScope(scope);
            return res;
        }
    }
    error("Malfored function invoke");
}

VALUE* evaluate(VALUE *val, NODE *scope) {
    debugVal(val,"Evaluate: ");
    if (!val) return NIL;
    switch (val->type) {
        case ID_NODE: {
            VALUE *func = evaluate(((NODE*)val)->data,scope);
            NODE *args = asNODE(((NODE*)val)->addr);
            VALUE *res = funcall(func,args,scope);
            debugVal(res,"Func result: ");
            decRef(func);
            return res;
        }
        case ID_SYMBOL:
            return resolve(((SYMBOL*)val),scope);
        default:
            incRef(val);
            return val;
    }
}
