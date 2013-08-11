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

#include "lisp.h"
#include "scope.h"
#include "primitives.h"
#include "parser.h"
#include "binmap.h"
#include "listops.h"
#include <string.h>

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
    error("Cannot copy a non-value");
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
            switch (((NODE*)val)->datatype) {
                case DATA_NODE:
                case DATA_FUNCTION:
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
                case DATA_SCOPE:
                    printf("SCOPE@%p ",(void*)val);
                    return;
            }
        case ID_INTEGER:
            printf("%i ", ((INTEGER*)val)->val);
            return;
        case ID_REAL:
            printf("%f ", ((REAL*)val)->val);
            return;
        case ID_SYMBOL:
            printf("%s ",sym_str((SYMBOL*)val));
            return;
        case ID_STRING:
            printf("\"%s\"",((STRING*)val)->str);
            return;
        case ID_PRIMFUNC:
            printf("%s ",prim_str((PRIMFUNC*)val));
            return;
    }
}

#define prim_func(name) { \
    NODE *args_eval = list(args,scope); \
    VALUE *res = (VALUE*)name(args_eval,scope); \
    decRef(args_eval); \
    return res; \
}

#define prim_spec(name) return (VALUE*)name(args,scope)

VALUE* call_function(VALUE *func, NODE *args, NODE *scope) {
    debugVal(func,"function form: ");
    failNIL(func,"NIL cannot be invoked");
    switch (func->type) {
        case ID_PRIMFUNC:
            switch (((PRIMFUNC*)func)->id) {
                case PRIM_LAMBDA: prim_spec(lambda);
                case PRIM_PROG: prim_spec(prog);
                case PRIM_LIST: prim_spec(list);
                case PRIM_QUOTE: prim_spec(quote);
                case PRIM_NODE: prim_func(node);
                case PRIM_ADDR: prim_func(addr);
                case PRIM_DATA: prim_func(data);
                case PRIM_SETD: prim_func(setd);
                case PRIM_SETA: prim_func(seta);
                case PRIM_REF: prim_func(ref);
                case PRIM_BIND: prim_func(bind);
                case PRIM_ADD: prim_func(add);
                case PRIM_SUB: prim_func(sub);
                case PRIM_MUL: prim_func(mul);
                case PRIM_DIV: prim_func(div_);
                default: error("Unhandled PRIMFUNC");
            }
        case ID_NODE: {
            NODE *fn_scope = scope_push(asNODE(((NODE*)func)->data));
            NODE *fn_vars = asNODE(asNODE(((NODE*)func)->addr)->data);
            if (!fn_vars->addr && fn_vars->data->type == ID_NODE) {
                fn_vars = asNODE(fn_vars->data); 
                scope_bindArgs(fn_vars,args,fn_scope); //quote args
            } else {
                NODE *fn_args = list(args,scope); //eval args
                scope_bindArgs(fn_vars,fn_args,fn_scope);
                decRef(fn_args);
            }
            NODE *fn_body = asNODE(asNODE(((NODE*)func)->addr)->addr);
            while (fn_body->addr) {
                decRef(evaluate(fn_body->data,fn_scope));
                fn_body = asNODE(fn_body->addr);
            }
            VALUE *res = evaluate(fn_body->data,fn_scope);
            scope_pop(fn_scope);
            return res;
        }
    }
    error("Malfored function invoke");
}

VALUE* evaluate(VALUE *val, NODE *scope) {
    debugVal(val,"evaluate: ");
    if (!val) return NIL;
    switch (val->type) {
        case ID_NODE: {
            VALUE *func = evaluate(((NODE*)val)->data,scope);
            NODE *args = asNODE(((NODE*)val)->addr);
            VALUE *res = call_function(func,args,scope);
            debugVal(res,"function result: ");
            decRef(func);
            return res;
        }
        case ID_SYMBOL: {
            VALUE *v = scope_resolve(((SYMBOL*)val),scope);
            return v;
        }
        default:
            incRef(val);
            return val;
    }
}

#define expandlist(list,parent) \
    for (NODE *expander = list, *last = NIL; expander; ) { \
        if (expander->data && expander->data->type == ID_NODE) { \
            if (expander->data) { \
                expander->data = macroexpand(asNODE(expander->data),scope,macros); \
                if (!expander->data) { \
                    if (last) { \
                        last->addr = expander->addr; \
                        expander->addr = NIL; \
                        decRef(expander); \
                        expander = (NODE*)last->addr; \
                        continue; \
                    } else { \
                        NODE *next = asNODE(list->addr); \
                        list->addr = NIL; \
                        decRef(list); \
                        list = next; \
                        expander = next; \
                        continue; \
                    } \
                } \
            } \
        } \
        expander = asNODE((last = expander)->addr); \
    } \
    (parent)->addr = (VALUE*)list;
    

VALUE* macroexpand(NODE *form, NODE *scope, NODE *macros) {
    debugVal(form,"macroexpand: ");
    if (!form) return NIL;
    if (form->data) {
        switch (form->data->type) {
            case ID_PRIMFUNC: { //handle special form syntax
                switch (((PRIMFUNC*)form->data)->id) {
                    case PRIM_QUOTE:
                        return (VALUE*)form; //return unmodified form
                    case PRIM_LAMBDA: {
                        NODE *body = asNODE(asNODE(form->addr)->addr);
                        expandlist(body,(NODE*)form->addr);
                        return (VALUE*)form; //return expanded form
                    }
                    case PRIM_MACRO: { 
                        if (list_length(form) < 3) error("MACRO takes at least three arguments");
                        SYMBOL *name = asSYMBOL(asNODE(form->addr)->data);
                        debugVal(name,"new macro: ")
                        incRef(name);
                        NODE *args = asNODE(asNODE(form->addr)->addr);
                        args->data = (VALUE*)newNODE(args->data,NIL);
                        incRef(args);
                        decRef(form);
                        NODE *body = asNODE(args->addr);
                        expandlist(body,args);
                        incRef(scope); 
                        NODE *macro = newNODE(scope,args); 
                        debugVal(macro,"macro func: ");
                        macro->datatype = DATA_FUNCTION; 
                        binmap_put(name,macro,macros);
                        return NIL; //emits no runtime code
                    }
                }
                NODE *args = asNODE(form->addr);
                expandlist(args,form);
                return (VALUE*)form; //return expanded form
            }
            case ID_SYMBOL: { //handle an invokable form w/ symbol
                debug("detected possible macro\n");
                NODE *args = asNODE(form->addr);
                debugVal(args,"macro arguments: ");
                expandlist(args,form);
                debugVal(args,"expanded macro arguments: ");
                NODE *macro = binmap_find(form->data,macros);
                if (macro) {
                    debugVal(form,"expanding: ");
                    VALUE *replace = call_function(macro->addr,args,scope);
                    decRef(form); 
                    return replace; //return replaced form
                } else {
                    return (VALUE*)form; //return expanded form
                }
            }
            case ID_NODE: { //handle dynamic function
                VALUE *head = macroexpand((NODE*)form->data,scope,macros);
                form->data = head;
                NODE *args = asNODE(form->addr);
                expandlist(args,form);
                return (VALUE*)form;
            }
            //if its not a (non-nil) NODE, SYMBOL, or PRIMFUN in the eval tree, it's a syntax error.
        }
    }
    error("Invalid syntax detected by MACROEXPAND");
}
