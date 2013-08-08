#include "parser.h"
#include "listops.h"
#include "primitives.h"
#include "binmap.h"
#include <cctype>

T_SYMBOL hash(char *sym) {
    T_SYMBOL hval = 0xAAAAAAAA;
    for (int i = 0; sym[i]; i++) hval ^= ((T_SYMBOL)sym[i]) << ((4*i)%24);
    return hval;
}

NODE *sym_map = binmap(newSYMBOL(hash("NIL")),newSTRING("NIL"));

T_SYMBOL intern(char *c_str) {
    c_str = strdup(c_str);
    for (int i = 0; c_str[i]; i++) c_str[i] = toupper(c_str[i]);
    debug("intern: %s %i\n",c_str,hash(c_str));
    SYMBOL *sym = newSYMBOL(hash(c_str));
    STRING *str = newSTRING(c_str);
    NODE *entry;
    while (entry = binmap_find(sym,sym_map)) {
        debugVal(entry,"matching: ");
        if (cmpSTRING((STRING*)entry->addr,str)) {
            decRef(entry);
            sym->sym++;
        } else {
            break;
        }
    }
    if (!entry) {
        debug("adding symbol: %s\n",c_str);
        binmap_put(sym,str,sym_map);
    }
    decRef(entry);
    decRef(sym);
    decRef(str);
    return sym->sym;
}

NODE* parse(char *&exp) {
    debug("Parse List: %s\n",exp);
    NODE *head = NIL;
    while (*exp) {
        switch (*(exp++)) {
            case '\'':
                list_push(newNODE(newPRIMFUNC(PRIM_QUOTE),parse(exp)),head);
            case '(':
                list_push(parse(exp),head);
                continue;
            case ')':
                debugVal(head,"ParseList: ");
                return list_reverse(head);
            case '\n':
            case '\r':
            case '\t':
            case ' ':
                continue;
            default: {
                char *sym = exp-1;
                debug("Start of literal: %s\n",sym);
                while (*exp && *exp != ' ' && *exp != ')') exp++;
                char old = *exp;
                *exp = 0;
                debug("Literal: %s\n",sym);
                switch (sym[0]) {
                    case '+':
                        if (!sym[1]) {
                            list_push(newPRIMFUNC(PRIM_ADD),head);
                            break;
                        }
                    case '-':
                        if (!sym[1]) {
                            list_push(newPRIMFUNC(PRIM_SUB),head);
                            break;
                        }
                        if (!isdigit(sym[1])) {
                            list_push(newSYMBOL(intern(sym)),head);
                            break;
                        }  
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        {
                            bool real = false;
                            char *scn = sym+1;
                            while (*scn) {
                                if (*scn == '.') { 
                                    real = true;
                                } else if (!isdigit(*scn)) {
                                    error("Malformed number character %c",*scn);
                                }
                                scn++;
                            }
                            if (real) {
                                list_push(newREAL(atof(sym)),head);
                            } else {
                                list_push(newINTEGER(atoi(sym)),head);
                            }
                        }
                        break;
                    case '*':
                        if (!sym[1]) {
                            list_push(newPRIMFUNC(PRIM_MUL),head);
                            break;
                        }
                    case '/':
                        if (!sym[1]) {
                            list_push(newPRIMFUNC(PRIM_DIV),head);
                            break;
                        }
                    case 'l':
                        if (!strcmp("list",sym)) {
                            list_push(newPRIMFUNC(PRIM_LIST),head);
                            break;
                        }
                    case 'q':
                        if (!strcmp("quote",sym)) {
                            list_push(newPRIMFUNC(PRIM_QUOTE),head);
                            break;
                        }
                    default:
                        list_push(newSYMBOL(intern(sym)),head);
                        break;
                 }
                 *exp = old;
                 debugVal(head,"ParseLiteral: ");
             }
        }
    }
    return list_reverse(head,NIL);
}

NODE* parseForms(char *exp) {
    char *dup = strdup(exp);
    char *org = dup;
    NODE *forms = parse(dup);
    free(org);
    return forms;
}
