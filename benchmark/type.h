#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdint.h>
#include <stdio.h>

typedef int rt_err_t;
typedef uintptr_t rt_ubase_t;
typedef size_t rt_size_t;

#if defined(container_of)
#undef container_of
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })
#else
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })
#endif

struct mm_aspace {};

#endif /* __TYPE_H__ */