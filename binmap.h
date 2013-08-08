#ifndef _BINMAP
#define _BINMAP

#include "lisp.h"

//node = ((key . val ) . (left . right))

NODE* binmap(void *pivot_key, void *pivot_val);
NODE* binmap_find(void *key, NODE *binmap);
void binmap_put(void *key, void *val, NODE *binmap);

#endif
