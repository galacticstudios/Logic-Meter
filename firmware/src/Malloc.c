
// To enable this code, the linker needs these switches on the command line:
// --wrap malloc --wrap free --wrap calloc --wrap realloc

#include <stdlib.h>

#define countof(x) (sizeof(x) / sizeof((x)[0]))

void *__real_malloc(size_t);
void __real_free(void *p);
void *__real_calloc(size_t, size_t);
void *__real_realloc(void *p, size_t);

unsigned long allocSizes[1000];
unsigned long totalAllocs;

void *__wrap_malloc(size_t sz)
{
    if (sz < countof(allocSizes))
        ++allocSizes[sz];
    else
        ++allocSizes[countof(allocSizes) - 1];
    totalAllocs += sz;
    size_t *p = (size_t *) __real_malloc(sz + sizeof(size_t));
    if (p == 0)
    {
        __builtin_software_breakpoint();
        return 0;
    }
    *p = sz;
    return p + 1;
}

void __wrap_free(void *p)
{
    size_t *c = (size_t *) p - 1;
    totalAllocs -= *c;
    __real_free(c);
}

void *__wrap_calloc(size_t a, size_t b)
{
    return malloc(a * b);
}

void *__wrap_realloc(void *p, size_t sz)
{
    size_t *c;
    if (p)
    {
        c = (size_t *) p - 1;
        if (*c < countof(allocSizes))
            ++allocSizes[*c];
        else
            ++allocSizes[countof(allocSizes) - 1];
        totalAllocs -= *c;
    }
    else
        c = 0;
    if (sz < countof(allocSizes))
        ++allocSizes[sz];
    else
        ++allocSizes[countof(allocSizes) - 1];
    totalAllocs += sz;
    c = (size_t *) __real_realloc(c, sz + sizeof(size_t));
    if (c == 0)
    {
        __builtin_software_breakpoint();
        return 0;
    }
    *c = sz;
    return c + 1;
}
