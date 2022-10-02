#ifndef __STACK_H__
#define __STACK_H__

#include <stdbool.h>

#define __USE_MISC
#include <dirent.h>

typedef struct stack {
    DIR* value;
    struct stack* next;
} stack_t;


bool stk_push(stack_t** stk, DIR* val);
DIR* stk_pop(stack_t** stk);

#endif /* __STACK_H__ */
