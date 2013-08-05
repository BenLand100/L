#include "parser.h"
#include "listops.h"
#include "primitives.h"

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

T_SYMBOL intern(char *sym) {
    return 0; //all symbols are the same for now
}

NODE* parse(char *&exp) {
    debug("Parse List: %s\n",exp);
    NODE *head = NIL;
    while (*exp) {
        switch (*(exp++)) {
            case '(':
                pushList(parse(exp),head);
                continue;
            case ')':
                debugVal(head,"ParseList: ");
                return reverseList(head);
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
                            pushList(newPRIMFUNC(PRIM_ADD),head);
                            break;
                        }
                    case '-':
                        if (!sym[1]) {
                            pushList(newPRIMFUNC(PRIM_SUB),head);
                            break;
                        }
                        if (!isDigit(sym[1])) {
                            pushList(newSYMBOL(intern(sym)),head);
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
                                } else if (!isDigit(*scn)) {
                                    error("Malformed number character %c",*scn);
                                }
                                scn++;
                            }
                            if (real) {
                                pushList(newREAL(atof(sym)),head);
                            } else {
                                pushList(newINTEGER(atoi(sym)),head);
                            }
                        }
                        break;
                    case '*':
                        if (!sym[1]) {
                            pushList(newPRIMFUNC(PRIM_MUL),head);
                            break;
                        }
                    case '/':
                        if (!sym[1]) {
                            pushList(newPRIMFUNC(PRIM_DIV),head);
                            break;
                        }
                    default:
                        pushList(newSYMBOL(intern(sym)),head);
                        break;
                 }
                 *exp = old;
                 debugVal(head,"ParseLiteral: ");
             }
        }
    }
    return reverseList(head,NIL);
}

void printList(NODE *list) {
    if (list->addr) {
        if (list->addr->type == ID_NODE) {
            print(list->data);
            printList((NODE*)list->addr);
        } else {
            print(asVALUE(list));
        }
    } else {    
        print(list->data);
    }
}

void print(VALUE *val) {
    if (!val) {
        printf("NIL ");
        return;
    }
    switch (val->type) {
        case ID_NODE:
            printf("( ");
            if (((NODE*)val)->addr && ((NODE*)val)->addr->type != ID_NODE) {
                print(((NODE*)val)->data);
                printf(". ");
                print(((NODE*)val)->addr);
            } else {
                printList(((NODE*)val));
            }
            printf(") ");
            return;
        case ID_INTEGER:
            printf("%i ", ((INTEGER*)val)->val);
            return;
        case ID_REAL:
            printf("%f ", ((REAL*)val)->val);
            return;
        case ID_SYMBOL:
            printf("SYMBOL_%i ",((SYMBOL*)val)->sym);
            return;
        case ID_PRIMFUNC:
            printf("PRIMFUNC_%i ",((PRIMFUNC*)val)->id);
            return;
    }
}
