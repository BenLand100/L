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
                case PRIM_LAMBDA: { incRef(args); incRef(scope); return newNODE(scope,args); }
                case PRIM_LIST: prim_spec(list);
                case PRIM_QUOTE: prim_spec(quote);
                case PRIM_ADDR: prim_func(addr);
                case PRIM_DATA: prim_func(data);
                case PRIM_SETD: prim_func(setd);
                case PRIM_SETA: prim_func(seta);
                case PRIM_REF: prim_spec(ref);
                case PRIM_ADD: prim_func(add);
                case PRIM_SUB: prim_func(sub);
                case PRIM_MUL: prim_func(mul);
                case PRIM_DIV: prim_func(div_);
                default: error("Unhandled PRIMFUNC");
            }
        case ID_NODE: {
            NODE *fn_scope = scope_push(asNODE(((NODE*)func)->data));
            NODE *vars = asNODE(asNODE(((NODE*)func)->addr)->data);
            NODE *args_eval = list(args,scope);
            scope_bindMany(vars,args_eval,fn_scope);
            decRef(args_eval);
            NODE *body = asNODE(asNODE(((NODE*)func)->addr)->addr);
            while (body->addr) {
                decRef(evaluate(body->data,fn_scope));
                body = asNODE(body->addr);
            }
            VALUE *res = evaluate(body->data,fn_scope);
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
        case ID_SYMBOL:
            return scope_resolve(((SYMBOL*)val),scope);
        default:
            incRef(val);
            return val;
    }
}

//TODO macro expansion
/*
VALUE* call_macro(NODE *macro, NODE *

NODE* macroexpand(NODE *form, NODE *scope) {
    debugVal(form,"macroexpand: ");
    if (!form) return NIL;
    switch (val->type) {
        case ID_NODE : {
            VALUE *func = evaluate(((NODE*)val)->data,scope);
            NODE *args = asNODE(((NODE*)val)->addr);
            VALUE *res = call_macro(func,args,scope);
            debugVal(res,"macro result: ");
            decRef(func);
            return res;
        }
        case ID_SYMBOL:
            return scope_resolve(((SYMBOL*)val),scope);
        default:
            incRef(val);
            return val;
    }
} 

*/
