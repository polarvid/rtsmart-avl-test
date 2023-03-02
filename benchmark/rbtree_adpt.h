#ifndef __MM_RBTREE_ADPT_H__
#define __MM_RBTREE_ADPT_H__

#include "../template/linux-rbtree.h"
#include <stdint.h>

#define VAREA_ENTRY(pnode)                                                      \
    (pnode)                                                                     \
        ? container_of(container_of(pnode, struct _aspace_node, node),          \
                       struct rt_varea, node)                                   \
        : 0
#define ASPACE_VAREA_NEXT(pva) (VAREA_ENTRY(rb_next(&pva->node.node)))
#define ASPACE_VAREA_FIRST(aspace) (VAREA_ENTRY(rb_first(&aspace->tree.tree)))

typedef struct _aspace_node
{
    struct rb_node node;
} *_aspace_node_t;

typedef struct _aspace_tree
{
    struct rb_root tree;
} *_aspace_tree_t;

#endif /* __MM_RBTREE_ADPT_H__ */
