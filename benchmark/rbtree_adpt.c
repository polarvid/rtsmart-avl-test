/* insider */

#ifdef _TEST_LINUXRB

#include <stddef.h>
#include "type.h"
#include "mm_aspace.h"
#include "mm_private.h"

#include "../template/linux-rbtree.h"

#define LOG_D(x,...)

rt_err_t _aspace_bst_init(struct rt_aspace *aspace)
{
    aspace->tree.tree = RB_ROOT;
    return 0;
}

static int compare_overlap(void *as, void *ae, void *bs, void *be)
{
    LOG_D("as %lx, ae %lx, bs %lx, be %lx", as, ae, bs, be);
    int cmp;
    if (as > be)
    {
        cmp = 1;
    }
    else if (ae < bs)
    {
        cmp = -1;
    }
    else
    {
        cmp = 0;
    }
    LOG_D("ret %d", cmp);
    return cmp;
}

static int compare_exceed(void *as, void *ae, void *bs, void *be)
{
    LOG_D("as %lx, ae %lx, bs %lx, be %lx", as, ae, bs, be);
    int cmp;
    if (as > bs)
    {
        cmp = 1;
    }
    else if (as < bs)
    {
        cmp = -1;
    }
    else
    {
        cmp = 0;
    }
    LOG_D("ret %d", cmp);
    return cmp;
}

static struct rt_varea *search(struct rb_root *root, struct _mm_range range,
                               int (*compare)(void *as, void *ae, void *bs, void *be))
{
    struct rb_node *node = root->rb_node;
    while (node)
    {
        rt_varea_t varea = VAREA_ENTRY(node);
        int cmp = compare(range.start, range.end, varea->start, varea->start + varea->size - 1);

        if (cmp < 0)
        {
            node = node->rb_left;
        }
        else if (cmp > 0)
        {
            node = node->rb_right;
        }
        else
        {
            return varea;
        }
    }
    return NULL;
}

struct rt_varea *_aspace_bst_search(struct rt_aspace *aspace, void *key)
{
    struct rb_root *root = &aspace->tree.tree;
    struct _mm_range range = {key, key};
    return search(root, range, compare_overlap);
}

rt_varea_t _aspace_bst_search_exceed(struct rt_aspace *aspace, void *start)
{
    struct rb_root *root = &aspace->tree.tree;
    struct rb_node *node = root->rb_node;
    rt_varea_t closest = NULL;
    ptrdiff_t min_off = PTRDIFF_MAX;
    while (node)
    {
        rt_varea_t varea = VAREA_ENTRY(node);
        void *va_s = varea->start;
        int cmp = compare_exceed(start, start, va_s, va_s);

        if (cmp < 0)
        {
            ptrdiff_t off = va_s - start;
            if (off < min_off)
            {
                min_off = off;
                closest = varea;
            }
            node = node->rb_left;
        }
        else if (cmp > 0)
        {
            node = node->rb_right;
        }
        else
        {
            return varea;
        }
    }
    return closest;
}

struct rt_varea *_aspace_bst_search_overlap(struct rt_aspace *aspace, struct _mm_range range)
{
    struct rb_root *root = &aspace->tree.tree;
    return search(root, range, compare_overlap);
}

void _aspace_bst_insert(struct rt_aspace *aspace, struct rt_varea *varea)
{
    struct rb_root *root = &aspace->tree.tree;
    struct rb_node *current = NULL;
    struct rb_node **next = &(root->rb_node);
    rt_ubase_t key = (rt_ubase_t)varea->start;

    /* Figure out where to put new node */
    while (*next)
    {
        current = *next;
        struct rt_varea *data = VAREA_ENTRY(current);

        if (key < (rt_ubase_t)data->start)
            next = &(current->rb_left);
        else if (key > (rt_ubase_t)data->start)
            next = &(current->rb_right);
        else
            return;
    }

    /* Add new node and rebalance tree. */
    rb_link_node(&varea->node.node, current, next);
    rb_insert_color(&varea->node.node, root);
    return;
}

void _aspace_bst_remove(struct rt_aspace *aspace, struct rt_varea *varea)
{
    struct rb_node *node = &varea->node.node;
    rb_erase(node, &aspace->tree.tree);
}

#endif /* 0 */
