#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mm_aspace.h"
#include "mm_private.h"

/* test library */
#include "random.h"
#include "shuffle.h"

/* time */
#include <time.h>

static int gettime(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME_COARSE, &ts);
    time_t seconds = ts.tv_sec;
    int millisecond = ts.tv_nsec / 1000000;
    return millisecond + seconds * 1000;
}

struct rt_aspace aspace;

/**
 * @brief Simulate environment of varea and BSTs
 */

/* test data set */
int *dataset;
int loop_count;

/* preallocate varea to decrease influence by malloc routine */
struct rt_varea *_varea_buf;

#define STOPWATCH(fun, time) do {   \
    unsigned int _time;             \
    _time = gettime();              \
    fun();                          \
    _time = gettime()-_time;        \
    time = _time;                   \
    } while (0);

static void init_test(void)
{
    _aspace_bst_init(&aspace);

    dataset = malloc(loop_count * sizeof(*dataset));
    assert(dataset);

    _varea_buf = malloc(loop_count * sizeof(*_varea_buf));
    assert(_varea_buf);

    init_random_keys(dataset, loop_count, 0xabcdabcd);
}

static void insert_test(void)
{
    for (size_t i = 0; i < loop_count; i++)
    {
        struct rt_varea *varea;
        varea = &_varea_buf[i];
        varea->start = (void *)(uintptr_t)dataset[i];
        varea->size = 1;
        _aspace_bst_insert(&aspace, varea);
    }
}

static void search_test(void)
{
    for (size_t i = 0; i < loop_count; i++)
    {
        void *start = (void *)(uintptr_t)dataset[i];
        struct rt_varea *varea;
        varea = _aspace_bst_search(&aspace, start);
        assert(varea);
        assert(varea->start == start);
    }
}

static void delete_test(void)
{
    for (size_t i = 0; i < loop_count; i++)
    {
        void *start = (void *)(uintptr_t)dataset[i];
        struct rt_varea *varea;
        varea = _aspace_bst_search(&aspace, start);
        _aspace_bst_remove(&aspace, varea);
    }
}

static void cleanup(void)
{
    free(dataset);

    free(_varea_buf);
}

int main(int argc, char *argv[])
{
    if (argc == 2)
    {
        sscanf(argv[1], "%d", &loop_count);
    }
    else
    {
        loop_count = 1000;
    }

    puts("Benchmark");
    printf("looping times: %d\n", loop_count);

    init_test();
    int endurance;
    STOPWATCH(insert_test, endurance);
    printf("Insertion: %d ms\n", endurance);

    randomize(dataset, loop_count);
    STOPWATCH(search_test, endurance);
    printf("Search: %d ms\n", endurance);

    randomize(dataset, loop_count);
    STOPWATCH(delete_test, endurance);
    printf("Delete: %d ms\n", endurance);

    cleanup();
    puts("Benchmark exit");
    return 0;
}
