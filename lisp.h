#ifndef _LISP
#define _LISP

#include <cstdlib>
#include <cstdio>
#include <cstring>

#define error(msg,...) { printf("ERROR: "); printf(msg, ##__VA_ARGS__); printf("\n"); exit(0); }
#define debug(msg,...) { printf(msg, ##__VA_ARGS__); }
#define failNIL(val,fail) if (!(val)) error(fail);

#define printVal(val) {print((VALUE*)val); printf("\n");}
#define debugVal(val,lbl,...) { printf(lbl, ##__VA_ARGS__); printVal(val); }

#define as_type(_type) \
    inline _type* as ## _type(void *val) { \
        if (!val || ((VALUE*)val)->type != ID_ ## _type) error(#_type" expected"); \
        return (_type*)val; \
    }
    
#define new_type(_type,_var) \
    inline _type* new ## _type() { \
        _type *val = (_type*)malloc(sizeof(_type));\
        val->type = ID_ ## _type; \
        return val; \
    } \
    inline _type* new ## _type(T_ ## _type _ ## _var) { \
        _type *val = new ## _type(); \
        val->_var = _ ## _var; \
        return val; \
    } 
    
#define def_type(_type,_var) \
    typedef struct { \
        T_TYPE type; \
        T_ ## _type _var; \
    } _type; \
    new_type(_type,_var) \
    as_type(_type) \

#define ID_NODE      0x00
#define ID_SYMBOL    0x01
#define ID_INTEGER   0x02
#define ID_REAL      0x03
#define ID_STRING    0x04
#define ID_PRIMFUNC  0x05
#define ID_DATA      0x06

#define NIL NULL

typedef unsigned char T_TYPE;
typedef unsigned int T_SYMBOL;
typedef unsigned int T_PRIMFUNC;
typedef int T_INTEGER;
typedef double T_REAL;
typedef char* T_STRING;
typedef void* T_DATA;

typedef struct {
    T_TYPE type;
} VALUE;
#define asVALUE(val) ((VALUE*)val)
VALUE* deep_copy(VALUE *val);

typedef struct {
    T_TYPE type;
    VALUE *addr,*data;    
} NODE;

inline NODE* asNODE(void *val) { 
    if (val && ((VALUE*)val)->type != ID_NODE) error("NODE expected");
    return (NODE*)val;
}

inline NODE* newNODE() {
    NODE *node = (NODE*)malloc(sizeof(NODE));
    node->type = ID_NODE;
    return node;
}

inline NODE* newNODE(void *data, void *addr) {
    NODE *node = newNODE();
    node->data = (VALUE*)data;
    node->addr = (VALUE*)addr;
    return node;
}

def_type(SYMBOL,sym);
def_type(INTEGER,val);
def_type(REAL,val);
def_type(PRIMFUNC,id);
def_type(STRING,str);

#include "listops.h"
#include "parser.h"

VALUE* evaluate(VALUE *val, NODE *scope);

#endif
