#ifndef _PARSER
#define _PARSER

#include "lisp.h"

T_SYMBOL intern(char *sym);
NODE* parseForms(char *exp);

#endif
