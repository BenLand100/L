#ifndef _LISTOPS
#define _LISTOPS

#include "lisp.h"

//these functions DO NOT inc or dec refc.

inline int list_length(NODE *list) {
    int i = 0;
    while (list) {
        i++;
        list = asNODE(list->addr);
    }
    return i;
}

inline NODE* list_join(NODE *a, NODE *b) {
    NODE *head = a;
    while (a->addr) a = asNODE(a->addr);
    a->addr = asVALUE(b);
    return head;
}

inline NODE* list_reverse(NODE *list, NODE *tail) {
    if (!list) return tail;
    NODE* next = asNODE(list->addr);
    list->addr = asVALUE(tail);
    return list_reverse(next, list);
}

inline void list_push(void *val, NODE *&list) {
    list = newNODE(val,list);
}

inline VALUE* list_pop(NODE *&list) {
    failNIL(list,"NIL cannot be popped");
    VALUE *val = list->data;
    list = asNODE(list->addr);
    return val;
}

inline VALUE* list_peak(NODE *&list) {
    failNIL(list,"NIL cannot be peaked");
    return list->data;
}

inline NODE* list_reverse(NODE *list) {
    return list_reverse(list,NIL);
}

#endif
