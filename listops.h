#ifndef _LISTOPS
#define _LISTOPS

#include "lisp.h"

inline int length(NODE *list) {
    int i = 0;
    while (list) {
        i++;
        list = asNODE(list->addr);
    }
    return i;
}

inline NODE* joinList(NODE *a, NODE *b) {
    NODE *head = a;
    while (a->addr) a = asNODE(a->addr);
    a->addr = asVALUE(b);
    return head;
}

inline NODE* reverseList(NODE *list, NODE *tail) {
    if (!list) return tail;
    NODE* next = asNODE(list->addr);
    list->addr = asVALUE(tail);
    return reverseList(next, list);
}

inline void pushList(void *val, NODE *&list) {
    list = newNODE(val,list);
}

inline VALUE* popList(NODE *&list) {
    failNIL(list,"NIL cannot be popped");
    VALUE *val = list->data;
    list = asNODE(list->addr);
    return val;
}

inline VALUE* peakList(NODE *&list) {
    failNIL(list,"NIL cannot be peaked");
    return list->data;
}

inline NODE* reverseList(NODE *list) {
    return reverseList(list,NIL);
}

#endif
