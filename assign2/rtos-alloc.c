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
    // printf("in Malloc\n");
    alloc_t *allocation;
    allocation = mmap(NULL, sizeof(allocation) + size, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    if (allocation == MAP_FAILED)
    {
        printf("Failed Mapping with error %d\n", errno);
    }
    // printf("allocated %p\n", allocation);
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
    struct allocation *allocation;
    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            allocation = ap;
            break;
        }
    }
    allocation->a_len = size;
    allocation = mmap((void *)allocation, sizeof(allocation) + size, PROT_READ | PROT_WRITE, MAP_PRIVATE, 0, 0);

    return allocation;
}

void rtos_free(void *ptr)
{
    // printf("first %p\n", first);
    // printf("last %p\n", last);
    // printf("prt %p\n", ptr);
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
        // printf("last\n");
        size_t len = last->a_len;
        last = last->a_prev;
        if (last == first)
        {
            first->a_next = NULL;
        }

        int z = munmap(ptr, sizeof(alloc_t) + len);
        // printf("%p\n", ptr);
        // printf("%d\n", z);
        return;
    }

    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        // printf("current ptr: %p\n", ap);

        if (ptr == ap)
        {
            // printf("found ptr\n");
            // printf("%p\n", ap->a_prev->a_next);
            alloc_t *prev = ap->a_prev;
            alloc_t *next = ap->a_next;
            prev->a_next = ap->a_next;
            next->a_prev = ap->a_prev;
            // printf("shifted\n");
            int z = munmap(ptr, sizeof(alloc_t) + ap->a_len);
            // printf("%p\n", ptr);
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
    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        printf("finding size\n");
        if (ptr == ap)
        {
            printf("alloc size: %ld\n", ap->a_len);
            return ap->a_len;
        }
    }
    printf("COULDNT FIND SIZE\n");
}

bool rtos_allocated(void *ptr)
{
    printf("first %p\n", first);
    printf("last %p\n", last);
    printf("prt %p\n", ptr);
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
    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            printf("allocated");
            return true;
        }
    }
    printf("not allocated");
    return false;
}

size_t rtos_total_allocated(void)
{
    if (first == NULL)
    {
        return 0;
    }
    size_t total = 0;
    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        total += ap->a_len;
    }
    total += last->a_len;
    printf("rtos Total %ld\n", total);
    return total;
}

bool rtos_is_valid(void *ptr)
{
    printf("first %p\n", first);
    printf("last %p\n", last);
    printf("prt %p\n", ptr);
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
    for (struct allocation *ap = first; ap->a_next != NULL; ap = ap->a_next)
    {
        if (ptr == ap)
        {
            printf("allocated");
            return true;
        }
    }
    printf("not allocated");
    return false;
}
