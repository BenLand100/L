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
#include "parser.h"

NODE* l_list(NODE *args, NODE *scope) {
    return args ? newNODE(evaluate(args->data,scope),l_list(asNODE(args->addr),scope)) : NIL;
}

VALUE* l_prog(NODE *args, NODE *scope) {
    VALUE *res = NIL;
    for (NODE *form = args; form; form = asNODE(form->addr)) {
        decRef(res);
        res = evaluate(form->data,scope);
    }
    return res;
}

VALUE* l_cond(NODE *args, NODE *scope) {
    while (args) {
        NODE *test = asNODE(args->data);
        if (list_length(test) != 2) error("Malformed conditional");
        VALUE *v = evaluate(test->data,scope);
        if (v) {
            decRef(v);
            return evaluate(asNODE(test->addr)->data,scope);
        }
        decRef(v);
        args = asNODE(args->addr);
    }
    return NIL;
}

VALUE* l_macro(NODE *args, NODE *scope) {
    error("MACRO called as a function.");
}

VALUE* l_lambda(NODE *args, NODE *scope) {
    incRef(args); 
    incRef(scope); 
    NODE *f = newNODE(scope,args); 
    f->datatype = DATA_FUNCTION; 
    return (VALUE*)f;
}

VALUE* l_quote(NODE *args, NODE *scope) {
    if (args->addr) error("QUOTE takes exactly 1 argument");
    return deep_copy(args->data);
}

VALUE* l_node(NODE *args, NODE *scope) {
    if (list_length(args) != 2) error("SETD takes exactly 2 arguments");
    incRef(args->data);
    incRef(((NODE*)args->addr)->data);
    return (VALUE*)newNODE(args->data,((NODE*)args->addr)->data);
}

VALUE* l_data(NODE *args, NODE *scope) {
    if (args->addr) error("DATA takes exactly 1 argument");
    VALUE *res = asNODE(args->data)->data;
    incRef(res);
    return res;
}

VALUE* l_addr(NODE *args, NODE *scope) {
    if (args->addr) error("ADDR takes exactly 1 argument");
    VALUE *res = asNODE(args->data)->addr;
    incRef(res);
    return res;
}

VALUE* l_setd(NODE *args, NODE *scope) {
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

VALUE* l_seta(NODE *args, NODE *scope) {
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

VALUE* l_ref(NODE *args, NODE *scope) {
    if (args->addr) error("REF takes exactly 1 argument");
    NODE *ref = scope_ref(asSYMBOL(args->data),scope);
    failNIL(ref,"Cannot reference unbound symbol");
    return (VALUE*)ref;
}

VALUE* l_bind(NODE *args, NODE *scope) {
    if (list_length(args) != 2) error("BIND takes exactly 2 arguments");
    scope_bind(asSYMBOL(args->data),asNODE(args->addr)->data,scope);
    incRef(asNODE(args->addr)->data);
    return asNODE(args->addr)->data;
}


VALUE* l_add(NODE *args, NODE *scope) {
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

VALUE* l_mul(NODE *args, NODE *scope) {
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

VALUE* l_sub(NODE *args, NODE *scope) {
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

VALUE* l_div(NODE *args, NODE *scope) {
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


VALUE* l_print(NODE *args, NODE *scope) {
    if (!args) {
        print(NIL);
        return NIL;
    }
    while (args->addr) {
        print(args->data);
        args = asNODE(args->addr);
    }
    print(args->data);
    printf("\n");
    incRef(args->data);
    return args->data;
}

VALUE* l_isnode(NODE *args, NODE *scope) {
    if (!args || args->addr) error("ISNODE takes exactly 1 argument");
    return args->data->type == ID_NODE ? (VALUE*)newSYMBOL(intern("T")) : NIL;
}
