#ifndef _PARSER
#define _PARSER

#include "lisp.h"

T_SYMBOL intern(char *sym);
NODE* parse(char *&exp);
void print(VALUE *val);

#endif
