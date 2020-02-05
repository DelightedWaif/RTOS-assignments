#include "rtos-alloc.h"
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>

typedef struct allocation
{
    size_t a_len;

    struct allocation *a_prev;
    struct allocation *a_next;
} alloc_t;

alloc_t *first, *last;

void *rtos_malloc(size_t size)
{
    alloc_t *allocation;
    allocation = mmap(NULL, sizeof(allocation) + size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (allocation == MAP_FAILED)
    {
        printf("Failed Mapping with error %d\n", errno);
    }
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
    allocation->a_len = size;
    return allocation;
}

void *rtos_realloc(void *ptr, size_t size)
{
    alloc_t *allocation;
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            allocation = ap;
            break;
        }
    }
    rtos_free(ptr);
    allocation = rtos_malloc(size);

    return allocation;
}

void rtos_free(void *ptr)
{
    if (ptr == NULL)
    {
        return;
    }
    if (ptr == first)
    {
        size_t len = first->a_len;
        if (first->a_next != NULL)
        {
            alloc_t *next = first->a_next;
            next->a_prev = NULL;
            first->a_next = NULL;
            first = next;
        }
        else if (first == last)
        {
            first = last = NULL;
        }
        int z = munmap(ptr, sizeof(alloc_t) + len);

        return;
    }
    if (ptr == last)
    {
        size_t len = last->a_len;
        last = last->a_prev;
        if (last == first)
        {
            first->a_next = NULL;
        }

        int z = munmap(ptr, sizeof(alloc_t) + len);
        return;
    }

    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            alloc_t *prev = ap->a_prev;
            alloc_t *next = ap->a_next;
            prev->a_next = ap->a_next;
            next->a_prev = ap->a_prev;
            int z = munmap(ptr, sizeof(alloc_t) + ap->a_len);
            return;
        }
    }
}

size_t rtos_alloc_size(void *ptr)
{
    if (ptr == first)
    {
        return first->a_len;
    }
    if (ptr == last)
    {
        return last->a_len;
    }
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            return ap->a_len;
        }
    }
}

bool rtos_allocated(void *ptr)
{
    if (ptr == NULL || first == NULL)
    {
        return false;
    }
    if (ptr == first || ptr == last)
    {
        return true;
    }
    else if (ptr != first && first->a_next == NULL)
    {
        return false;
    }
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            return true;
        }
    }
    return false;
}

size_t rtos_total_allocated(void)
{
    if (first == NULL)
    {
        return 0;
    }
    size_t total = 0;
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        total += ap->a_len;
    }
    total += last->a_len;
    return total;
}

bool rtos_is_valid(void *ptr)
{
    if (ptr == NULL || first == NULL)
    {
        return false;
    }
    if (ptr == first || ptr == last)
    {
        return true;
    }
    else if (ptr != first && first->a_next == NULL)
    {
        return false;
    }
    for (alloc_t *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            return true;
        }
    }
    return false;
}
