#pragma once

struct cookie
{
    const char *value;
    struct cookie *next;
};

void push_cookie(struct cookie **cookie, const char* value);
int pop_cookie(struct cookie **cookie, const char** value);

void free_cookies(struct cookie **cookie);
