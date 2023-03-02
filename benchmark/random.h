#ifndef __RAMDOM_H__
#define __RAMDOM_H__

#include <stdlib.h>

#define RANDOM(n) (xrand() % (n))
static unsigned int xseed = 0x11223344;

static inline unsigned int xrand(void)
{
	return (((xseed = xseed * 214013L + 2531011L) >> 16) & 0x7fffffff);
}

// generate keys
static inline void init_random_keys(int *keys, int count, int seed)
{
	int save_seed = xseed;
	int *array = (int*)malloc(sizeof(int) * count);
	int length = count, i;
	xseed = seed;
	for (i = 0; i < count; i++) {
		array[i] = i;
	}
	for (i = 0; i < length; i++) {
		int pos = xrand() % count;
		int key = array[pos];
		keys[i] = key;
		array[pos] = array[--count];
	}
	free(array);
	xseed = save_seed;
}

#endif /* __RAMDOM_H__ */
