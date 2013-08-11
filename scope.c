/**
 *  Copyright 2013 by Benjamin J. Land (a.k.a. BenLand100)
 *
 *  This file is part of L, a virtual machine for a lisp-like language.
 *
 *  L is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  L is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with L. If not, see <http://www.gnu.org/licenses/>.
 */

#include "scope.h"
#include "binmap.h"
#include "parser.h"

NODE* scope_push(NODE *parent_scope) {
    incRef(parent_scope);
    NODE *scope = newNODE(NIL,parent_scope);
    scope->datatype = DATA_SCOPE;
    return scope;
}

NODE* scope_pop(NODE *scope) { 
    NODE *parent_scope = ((NODE*)scope->addr);
    decRef(scope);
    return parent_scope;
}

NODE* scope_ref(SYMBOL *sym, NODE *scope) {
    debug("resolving: %s\n", sym_str(sym));
    while (scope) {
        debugVal(scope,"scope: ");
        NODE *entry = binmap_find(sym,(NODE*)scope->data);
        if (entry) return entry;
        scope = (NODE*)scope->addr;
    }
    return NIL;
}

VALUE* scope_resolve(SYMBOL *sym, NODE *scope) {
    NODE *ref = scope_ref(sym,scope);
    failNIL(ref,"Unbound symbol: %s", sym_str(sym));
    VALUE *val = ref->addr;
    incRef(val);
    decRef(ref);
    return val;
}

void scope_bind(SYMBOL *sym, VALUE *val, NODE *scope) {
    debugVal(val,"Binding %s => ", sym_str(sym));
    incRef(val);
    incRef(sym);
    if (scope->data) {
        binmap_put(sym,val,(NODE*)scope->data);
    } else {
        scope->data = (VALUE*)binmap(sym,val);
    }
}

static bool scope_init_syms_flag = false;
static T_SYMBOL sym_rest;
static T_SYMBOL sym_optional;
void scope_init_syms() {
    scope_init_syms_flag = true;
    sym_rest = intern("&REST");
    sym_optional = intern("&OPTIONAL");
}

void scope_bindArgs(NODE *vars, NODE *vals, NODE *scope) {        
    if (!scope_init_syms_flag) scope_init_syms();
    bool optional = false;
    while (vars) {
        T_SYMBOL sym = asSYMBOL(vars->data)->sym;
        if (sym == sym_rest) {
            vars = asNODE(vars->addr);
            if (vars->addr) error("&REST argument must be last");
            incRef(vals);
            scope_bind(asSYMBOL(vars->data),(VALUE*)vals,scope);
            return;
        } else if (sym == sym_optional) {
            vars = asNODE(vars->addr);
            optional = true;
            scope_bind((SYMBOL*)vars->data,vals ? vals->data : NIL,scope);
        } else if (optional) {
            scope_bind((SYMBOL*)vars->data,vals ? vals->data : NIL,scope);
        } else {
            if (!vals) error("Not enough arguments to fill variables");
            scope_bind((SYMBOL*)vars->data,vals->data,scope);
        }
        vars = asNODE(vars->addr);
        if (vals) vals = asNODE(vals->addr);
    }
    if (vals) error("Too many arguments to fill variables");
}
