#include "rtos-alloc.h"
#include <err.h>
#include <time.h>
#include <stdio.h>

void tests()
{
	void *ptr = rtos_malloc(8);
	if (!rtos_allocated(ptr))
		printf("allocated pointer should be valid\n");
	if (rtos_alloc_size(ptr) < 8)
		printf("allocation should be no smaller than requested\n");
	if (rtos_allocated(NULL))
		printf("NULL is not a valid allocation\n");

	void *pt = rtos_malloc(16);
	if (!rtos_allocated(pt))
		printf("allocated pointer should be valid\n");
	printf("entering Free\n");
	rtos_free(pt);
	printf("done free\n");
	printf("entering allocated\n");
	if (rtos_allocated(pt))
		printf("pointer should not be valid after freeing it\n");
	printf("done allocated\n");
	rtos_free(NULL);
	printf("done free");
	if (rtos_allocated(NULL))
		printf("NULL is not a valid allocation\n");
};

void test_free()
{
	printf("in tests\n");
	void *p = rtos_malloc(8);
	printf("Address of p1: %p\n", p);
	printf("out of malloc\n");
	bool x = rtos_allocated(p);
	if (x)
	{
		printf("its allocated\n");
	}
	rtos_free(p);
	printf("free done\n");
	x = rtos_allocated(p);
	if (!x)
	{
		printf("free worked I think\n");
	}
	else
	{
		printf("free bad\n");
	}
}

void test_multiple_allocations()
{
	size_t total = rtos_total_allocated();
	printf("total %ld\n", total);

	printf("creating a bunch of pointers\n");
	void *pointers[6];
	size_t s[] = {1, 2, 17, 42, 1049, 50};
	for (size_t i = 0; i < 6; i++)
	{
		void *p = rtos_malloc(s[i]);
		size_t size = rtos_alloc_size(p);
		printf("individual size %ld\n", size);
		if (size < s[i])
			printf("allocation size %ld must be >= requested %ld\n", size, s[i]);

		pointers[i] = p;
		total += size;
	}
	printf("Total %ld\n", total);
	if (total == rtos_total_allocated())
	{
		printf("total correct\n");
	}
	else
	{
		printf("Incorrect total\n");
	}

	for (size_t i = 0; i < 6; i++)
	{
		printf("%ld\n", i);
		rtos_free(pointers[i]);
	}
}

void time_calcs()
{
	struct timespec begin, end;
	int x_array[30];
	void *p;
	for (int i = 0; i < 30; i++)
	{
		x_array[i] = rand() % (65 + 1 - 0) + 0;
	}
	printf("my malloc");
	for (int i = 0; i < 30; i++)
	{
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);
		p = rtos_malloc(x_array[i]);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		printf("%ld\n", end.tv_nsec - begin.tv_nsec);
		rtos_free(p);
	}
	printf("Good malloc");
	for (int i = 0; i < 30; i++)
	{
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &begin);
		p = malloc(x_array[i]);
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
		printf("%ld\n", end.tv_nsec - begin.tv_nsec);
		free(p);
	}

	printf("x");
	for (size_t i = 0; i < 30; i++)
	{
		printf("%d\n", x_array[i]);
	}
}

int main(int argc, char *argv[])
{
	printf("in main\n");
	time_calcs();
	return 0;
}
