#include "scope.h"
#include "binmap.h"

NODE* pushScope(NODE *parent_scope) {
    incRef(parent_scope);
    return newNODE(NIL,parent_scope);
}

NODE* popScope(NODE *scope) { 
    NODE *parent_scope = ((NODE*)scope->addr);
    decRef(scope);
    return parent_scope;
}

NODE* resolve_ref(SYMBOL *sym, NODE *scope) {
    debug("Resolving: %i\n", sym->sym);
    while (scope) {
        NODE *entry = binmap_find(sym,(NODE*)scope->data);
        if (entry) return entry;
        scope = (NODE*)scope->addr;
    }
    return NIL;
}

VALUE* resolve(SYMBOL *sym, NODE *scope) {
    NODE *ref = resolve_ref(sym,scope);
    if (ref) {
        VALUE *val = ref->addr;
        incRef(val);
        decRef(ref);
        return val;
    }
    error("Unbound symbol");
}

void bind(SYMBOL *sym, VALUE *val, NODE *scope) {
    debug("Binding: %i\n", sym->sym);
    incRef(val);
    if (scope->data) {
        binmap_put(sym,val,(NODE*)scope->data);
    } else {
        scope->data = (VALUE*)binmap(sym,val);
    }
}

void bindMany(NODE *vars, NODE *vals, NODE *scope) {
    while (vars && vals) {
        bind(asSYMBOL(vars->data),vals->data,scope);
        vars = asNODE(vars->addr);
        vals = asNODE(vals->addr);
    }
}
