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

#include "primitives.h"
#include "listops.h"
#include "scope.h"

NODE* list(NODE *args, NODE *scope) {
    return args ? newNODE(evaluate(args->data,scope),list(asNODE(args->addr),scope)) : NIL;
}

VALUE* prog(NODE *args, NODE *scope) {
    VALUE *res = NIL;
    for (NODE *form = args; form; form = asNODE(form->addr)) {
        decRef(res);
        res = evaluate(form->data,scope);
    }
    return res;
}

VALUE* lambda(NODE *args, NODE *scope) {
    incRef(args); 
    incRef(scope); 
    NODE *f = newNODE(scope,args); 
    f->datatype = DATA_FUNCTION; 
    return (VALUE*)f;
}

VALUE* quote(NODE *args, NODE *scope) {
    if (args->addr) error("QUOTE takes exactly 1 argument");
    return deep_copy(args->data);
}

VALUE* data(NODE *args, NODE *scope) {
    if (args->addr) error("DATA takes exactly 1 argument");
    VALUE *res = asNODE(args->data)->data;
    incRef(res);
    return res;
}

VALUE* addr(NODE *args, NODE *scope) {
    if (args->addr) error("ADDR takes exactly 1 argument");
    VALUE *res = asNODE(args->data)->addr;
    incRef(res);
    return res;
}

VALUE* setd(NODE *args, NODE *scope) {
    if (list_length(args) != 2) error("SETD takes exactly 2 arguments");
    NODE *n = asNODE(args->data);
    VALUE *v = asNODE(args->addr)->data;
    failNIL(n,"NIL is not a NODE");
    decRef(n->data);
    incRef(v);
    incRef(v);
    n->data = v;
    return v;
}

VALUE* seta(NODE *args, NODE *scope) {
    if (list_length(args) != 2) error("SETA takes exactly 2 arguments");
    NODE *n = asNODE(args->data);
    VALUE *v = asNODE(args->addr)->data;
    failNIL(n,"NIL is not a NODE");
    decRef(n->addr);
    incRef(v);
    incRef(v);
    n->addr = v;
    return v;
}

VALUE* ref(NODE *args, NODE *scope) {
    if (args->addr) error("REF takes exactly 1 argument");
    NODE *ref = scope_ref(asSYMBOL(args->data),scope);
    failNIL(ref,"Cannot reference unbound symbol");
    return (VALUE*)ref;
}

VALUE* bind(NODE *args, NODE *scope) {
    if (list_length(args) != 2) error("BIND takes exactly 2 arguments");
    scope_bind(asSYMBOL(args->data),asNODE(args->addr)->data,scope);
    incRef(asNODE(args->addr)->data);
    return asNODE(args->addr)->data;
}


VALUE* add(NODE *args, NODE *scope) {
    debugVal(args,"add: ");
    bool real = false;
    T_REAL accum_r = 0;
    T_INTEGER accum_i = 0;
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r += ((REAL*)args->data)->val;
            } else {
                accum_r += asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i + ((REAL*)args->data)->val;
            } else {
                accum_i += asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* mul(NODE *args, NODE *scope) {
    debugVal(args,"mul: ");
    bool real = false;
    T_REAL accum_r = 1.0;
    T_INTEGER accum_i = 1;
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r *= ((REAL*)args->data)->val;
            } else {
                accum_r *= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i * ((REAL*)args->data)->val;
            } else {
                accum_i *= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* sub(NODE *args, NODE *scope) {
    debugVal(args,"sub: ");
    bool real = false;
    T_REAL accum_r;
    T_INTEGER accum_i;
    failNIL(args->data,"NIL is not a number");
    if (args->data->type == ID_REAL) {
        accum_r = ((REAL*)args->data)->val;
        real = true;
    } else {
        accum_i = asINTEGER(args->data)->val;
    }
    args = asNODE(args->addr);
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r -= ((REAL*)args->data)->val;
            } else {
                accum_r -= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i - ((REAL*)args->data)->val;
            } else {
                accum_i -= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}

VALUE* div_(NODE *args, NODE *scope) {
    debugVal(args,"div: ");
    bool real = false;
    T_REAL accum_r;
    T_INTEGER accum_i;
    failNIL(args->data,"NIL is not a number");
    if (args->data->type == ID_REAL) {
        accum_r = ((REAL*)args->data)->val;
        real = true;
    } else {
        accum_i = asINTEGER(args->data)->val;
    }
    args = asNODE(args->addr);
    while (args) {
        failNIL(args->data,"NIL is not a number");
        if (real) {
            if (args->data->type == ID_REAL) {
                accum_r /= ((REAL*)args->data)->val;
            } else {
                accum_r /= asINTEGER(args->data)->val;
            }
        } else {
            if (args->data->type == ID_REAL) {
                real = true;
                accum_r = accum_i / ((REAL*)args->data)->val;
            } else {
                accum_i /= asINTEGER(args->data)->val;
            }
        }
        args = asNODE(args->addr);
    }    
    return real ? asVALUE(newREAL(accum_r)) : asVALUE(newINTEGER(accum_i));
}
