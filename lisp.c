#include "lisp.h"
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

VALUE* resolve(SYMBOL *sym, NODE *scope) {
    debug("Resolving: %i\n", sym->sym);
    while (scope) {
        if (asSYMBOL(asNODE(scope->data)->data)->sym == sym->sym) {
            incRef(((NODE*)scope->data)->addr);
            return ((NODE*)scope->data)->addr;
        }
        scope = asNODE(scope->addr);
    }
    return NIL;
}

NODE* bind(SYMBOL *sym, VALUE *val, NODE *scope) {
    incRef(val);
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
            VALUE *res = funcall(func,args,scope);
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
