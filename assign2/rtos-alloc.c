#include "rtos-alloc.h"
#include <sys/mman.h>

typedef struct allocation
{
    void *a_base;
    size_t a_len;

    struct allocation *a_prev;
    struct allocation *a_next;
} alloc_t;

alloc_t *first, *last;

void *rtos_malloc(size_t size)
{
    alloc_t *allocation;
    allocation->a_base = mmap((void *)allocation, size, PROT_READ|PROT_WRITE, MAP_PRIVATE, 0, 0);
    
    if (first == NULL)
    {
        first = last = allocation;
    }
    else
    {
        last->a_next = allocation;
        allocation->a_prev = last;
        last = allocation;
    }
    return allocation->a_base;
}

void *rtos_realloc(void *ptr, size_t size)
{
    alloc_t *allocation;
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap->a_base)
        {
            allocation = ap;
            break;
        }
    }
    // remove allocation and allocate more
}

void rtos_free(void *ptr)
{
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap->a_base)
        {
            munmap(ptr, ap->a_len);
            ap->a_prev->a_next = ap->a_next;
            ap->a_next->a_prev = ap->a_prev;
        }
    }
}

size_t rtos_alloc_size(void *ptr)
{
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap->a_base)
        {
            munmap(ptr, ap->a_len);
        }
    }
}

bool rtos_allocated(void *ptr)
{
    alloc_t allocation;
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap->a_base)
        {
            return true;
        }
    }
    return false;
}

size_t rtos_total_allocated(void)
{
    size_t allocation;
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        allocation += ap->a_len;
    }
    return allocation;
}
