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

#ifndef _LISP
#define _LISP

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define bool    int
#define true    1
#define false   0

#define error(...) { printf("ERROR: "); printf(__VA_ARGS__); printf("\n"); exit(0); }
#define debug(...) { printf(__VA_ARGS__); }
#define failNIL(val,...) if (!(val)) error(__VA_ARGS__);

#define printVal(val) {print((VALUE*)val); printf("\n");}
#define debugVal(val,...) { printf(__VA_ARGS__); printVal(val); }

#define as_type(_type) \
    static inline _type* as ## _type(void *val) { \
        if (!val || ((VALUE*)val)->type != ID_ ## _type) error(#_type" expected"); \
        return (_type*)val; \
    }
    
#define new_type(_type,_var) \
    static inline _type* new ## _type(T_ ## _type _ ## _var) { \
        _type *val = (_type*)malloc(sizeof(_type));\
        val->type = ID_ ## _type; \
        val->refc = 1; \
        val->_var = _ ## _var; \
        return val; \
    } 

#define test_cmp(_type,_var,_cmp_cond) \
    static inline int cmp ## _type(_type *a, _type *b) { \
        return (_cmp_cond); \
    }    

#define def_type(_type,_var,_cmp_cond) \
    typedef struct { \
        T_TYPE type; \
        size_t refc; \
        T_ ## _type _var; \
    } _type; \
    new_type(_type,_var) \
    as_type(_type) \
    test_cmp(_type,_var,_cmp_cond)

#define ID_NODE      0x00
#define ID_SYMBOL    0x01
#define ID_INTEGER   0x02
#define ID_REAL      0x03
#define ID_STRING    0x04
#define ID_PRIMFUNC  0x05

#define NIL NULL

#define DATA_NODE       0x00
#define DATA_FUNCTION   0x01
#define DATA_SCOPE      0x02

typedef unsigned char T_TYPE;
typedef unsigned int T_SYMBOL;
typedef unsigned int T_PRIMFUNC;
typedef int T_INTEGER;
typedef double T_REAL;
typedef char* T_STRING;
typedef void* T_DATA;

typedef struct {
    T_TYPE type;
    size_t refc;
} VALUE;

#ifdef GC_DEBUG
    #define incRef(val) if (val) { \
        debug("incref(%p):%u\n",(void*)val,((VALUE*)val)->refc); \
        ++(((VALUE*)val)->refc); \
    }
    #define decRef(val) if (val) { \
        debug("decref(%p):%u\n",(void*)val,(int)((VALUE*)val)->refc); \
        if (--(((VALUE*)val)->refc) == 0) { \
            debugVal(val,"free: "); \
            freeVALUE((VALUE*)val); \
        } \
    }
#else 
    #define incRef(val) if (val) { ++(((VALUE*)val)->refc); }
    #define decRef(val) if (val) { if (--(((VALUE*)val)->refc) == 0) { freeVALUE((VALUE*)val); } }
#endif

#define asVALUE(val) ((VALUE*)val)
void freeVALUE(VALUE *val);
VALUE* deep_copy(VALUE *val);

typedef struct {
    T_TYPE type;
    size_t refc;
    T_TYPE datatype;
    VALUE *addr,*data;    
} NODE;

static inline NODE* asNODE(void *val) { 
    if (val && ((VALUE*)val)->type != ID_NODE) error("NODE expected");
    return (NODE*)val;
}

static inline NODE* newNODE(void *data, void *addr) {
    NODE *node = (NODE*)malloc(sizeof(NODE));
    node->type = ID_NODE;
    node->refc = 1;
    node->datatype = DATA_NODE;
    node->data = (VALUE*)data;
    node->addr = (VALUE*)addr;
    return node;
}

static inline int cmpNODE(NODE *a, NODE *b) {
    return (size_t)a - (size_t)b;
}

def_type(SYMBOL,sym,(int)a->sym - (int)b->sym)
def_type(INTEGER,val,a->val - b->val)
def_type(REAL,val,a->val - b->val)
def_type(PRIMFUNC,id,(int)a->id - (int)b->id)
def_type(STRING,str,strcmp(a->str,b->str))

static inline int cmpVALUE(void *_a, void *_b) {
    VALUE *a = asVALUE(_a);
    VALUE *b = asVALUE(_b);
    if (a && b && a->type == b->type) {
        switch (a->type) {
            case ID_NODE:
                return cmpNODE((NODE*)a,(NODE*)b);
            case ID_SYMBOL:
                return cmpSYMBOL((SYMBOL*)a,(SYMBOL*)b);
            case ID_INTEGER:
                return cmpINTEGER((INTEGER*)a,(INTEGER*)b);
            case ID_REAL:
                return cmpREAL((REAL*)a,(REAL*)b);
            case ID_STRING:
                return cmpSTRING((STRING*)a,(STRING*)b);
            case ID_PRIMFUNC:
                return cmpPRIMFUNC((PRIMFUNC*)a,(PRIMFUNC*)b);
        }
    }
    error("Cannot compare dissimilar types\n");
}

VALUE* evaluate(VALUE *val, NODE *scope);
void print(VALUE *val);

#endif
