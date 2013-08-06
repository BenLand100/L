#ifndef _BINMAP
#define _BINMAP

#include "lisp.h"

//node = ((key . val ) . (left . right))

NODE *binmap(void *pivot_key, void *pivot_val);
NODE *find(void *key, NODE *binmap);
void put(void *key, void *val, NODE *binmap);

#endif
