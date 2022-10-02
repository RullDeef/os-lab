#include "stack.h"
#include <stdlib.h>


bool stk_push(stack_t** stk, DIR* val) {
    stack_t* new_node = malloc(sizeof(stack_t));
    bool status = new_node != NULL;

    if (status) {
        new_node->value = val;
        new_node->next = *stk;

        *stk = new_node;
    }

    return status;
}

DIR* stk_pop(stack_t** stk) {
    DIR* val = NULL;

    if (*stk != NULL) {
        val = (*stk)->value;
        stack_t* top = *stk;

        *stk = top->next;
        free(top);
    }

    return val;
}
