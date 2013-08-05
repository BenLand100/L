#include "lisp.h"
#include "primitives.h"

VALUE* deep_copy(VALUE *val) {
    return val; //for testing don't worry about copying
}

VALUE* resolve(SYMBOL *sym, NODE *scope) {
    debug("Resolving: %i\n", sym->sym);
    while (scope) {
        if (asSYMBOL(asNODE(scope->data)->data)->sym == sym->sym) {
            return ((NODE*)scope->data)->addr;
        }
        scope = asNODE(scope->addr);
    }
    return NIL;
}

NODE* bind(SYMBOL *sym, VALUE *val, NODE *scope) {
    return newNODE(newNODE(sym,val),scope);
}

NODE* bindMany(NODE *vars, NODE *vals, NODE *scope) {
    while (vars && vals) {
        scope = bind(asSYMBOL(vars->data),vals->data,scope);
        vars = asNODE(vars->addr);
        vals = asNODE(vals->addr);
    }
    return scope;
}

VALUE* funcall(VALUE *func, NODE *args, NODE *parent_scope) {
    debugVal(func,"Funcall: ");
    failNIL(func,"NIL cannot be invoked");
    switch (func->type) {
        case ID_PRIMFUNC:
            switch (((PRIMFUNC*)func)->id) {
                case PRIM_LIST:
                    return asVALUE(list(args,parent_scope));
                case PRIM_QUOTE:
                    return quote(args,parent_scope);
                case PRIM_ADDR:
                    return addr(list(args,parent_scope),parent_scope);
                case PRIM_DATA:
                    return data(list(args,parent_scope),parent_scope);
                case PRIM_ADD:
                    return add(list(args,parent_scope),parent_scope);
                case PRIM_SUB:
                    return sub(list(args,parent_scope),parent_scope);
                case PRIM_MUL:
                    return mul(list(args,parent_scope),parent_scope);
                case PRIM_DIV:
                    return div(list(args,parent_scope),parent_scope);
                default:
                    error("Unhandled PRIMFUNC");
            }
        case ID_NODE:
            NODE *vars = asNODE(((NODE*)func)->data);
            NODE *scope = bindMany(vars,list(args,parent_scope),parent_scope);
            return evaluate(((NODE*)func)->addr,scope);
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
            return funcall(func,args,scope);
        }
        case ID_SYMBOL: {
            return resolve(((SYMBOL*)val),scope);
        }
        default: {
            return deep_copy(val);
        }
    }
}

int main() {
    char *prog = strdup("( + 3 5 ) ( - 3 5 ) ( * 3 5 ) ( / 3.0 5 )");
    NODE *forms = parse(prog);
    printf("Total Forms = %i\n",length(forms));
    while (forms) {
        print(forms->data); printf("\n");
        NODE *static_scope = NULL;
        static_scope = bind(newSYMBOL(intern("+")),asVALUE(newPRIMFUNC(PRIM_ADD)),static_scope);
        VALUE *val = evaluate(asVALUE(forms->data),static_scope);
        printVal(val);
        forms = asNODE(forms->addr);
    }
}
