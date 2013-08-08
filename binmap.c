#include "binmap.h"

//entry = (key . val)
//tree = (left . right)
//node = (entry . tree)

#define binmap_entry(binmap) ((NODE*)(binmap)->data)
#define binmap_key(binmap) binmap_entry(binmap)->data
#define binmap_val(binmap) binmap_entry(binmap)->addr
#define binmap_tree(binmap) ((NODE*)(binmap)->addr)
#define binmap_left(binmap) ((NODE*)binmap_tree(binmap)->data)
#define binmap_right(binmap) ((NODE*)binmap_tree(binmap)->addr)

NODE* binmap(void *key, void *val) {
    return newNODE(newNODE(key,val),newNODE(NIL,NIL));
}

NODE* binmap_find(void *key, NODE *_binmap) {
    if (!_binmap) return NIL;
    int cmp = cmpVALUE(asVALUE(key),binmap_key(_binmap));
    if (!cmp) {
        NODE *entry = binmap_entry(_binmap);
        incRef(entry);
        return entry;
    } else if (cmp > 0) {
        return binmap_find(key,binmap_right(_binmap));
    } else {
        return binmap_find(key,binmap_left(_binmap));
    }
}

void binmap_put(void *key, void *val, NODE *_binmap) {
    failNIL(binmap,"BINMAP is NIL");
    int cmp = cmpVALUE(asVALUE(key),binmap_key(_binmap));
    if (!cmp) {
        decRef(binmap_val(_binmap));
        binmap_val(_binmap) = asVALUE(val);
    } else if (cmp > 0) {
        if (binmap_right(_binmap)) { 
            binmap_put(key,val,binmap_right(_binmap));
        } else {
            decRef(binmap_tree(_binmap)->addr);
            binmap_tree(_binmap)->addr = asVALUE(binmap(key,val));
        }
    } else {
        if (binmap_left(_binmap)) {
            binmap_put(key,val,binmap_left(_binmap));
        } else {
            decRef(binmap_tree(_binmap)->data);
            binmap_tree(_binmap)->data = asVALUE(binmap(key,val));
        }
    }
}  


