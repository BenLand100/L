#ifndef _SCOPE
#define _SCOPE

#include "lisp.h"

// scope = (symtree . parent_scope)

NODE* pushScope(NODE *parent_scope);
NODE* popScope(NODE *scope);

VALUE* resolve(SYMBOL *sym, NODE *scope);
void bind(SYMBOL *sym, VALUE *val, NODE *scope);
void bindMany(NODE *syms, NODE *vals, NODE *scope);

#endif
