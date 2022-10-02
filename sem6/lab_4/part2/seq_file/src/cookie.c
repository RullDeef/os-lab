#include "cookie.h"
#include <linux/vmalloc.h>

void push_cookie(struct cookie **cookie, const char* value)
{
    struct cookie *new_cookie = vmalloc(sizeof(struct cookie));
    new_cookie->value = value;
    new_cookie->next = *cookie;
    *cookie = new_cookie;
}

int pop_cookie(struct cookie **cookie, const char** value)
{
    if (*cookie == NULL)
        return 1;
    else if ((*cookie)->next != NULL)
        return pop_cookie(&(*cookie)->next, value);
    else
    {
        *value = (*cookie)->value;
        vfree(*cookie);
        *cookie = NULL;
        return 0;
    }
}

void free_cookies(struct cookie **cookie)
{
    const char *tmp = NULL;
    while (pop_cookie(cookie, &tmp) == 0)
        vfree(tmp);
}

