#ifndef __BST_ASSERT
#define __BST_ASSERT

/**
 * Here is the assertions to ensure rightness of bst maintenance
 * After each insertion and delete, a tree must still be binary search tree,
 * and still remain balanced
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "mm_aspace.h"
#include "type.h"

#define BUF_SIZE 1000000
static void *_start;
static void *_boundary;
static int _count;
static rt_varea_t _buf[BUF_SIZE];
#define RT_ASSERT assert

static void _print_varea(rt_varea_t varea, int depth)
{
    if (depth == 0)
    {
        printf("%p ", varea->start);
    }
    else
    {
        rt_varea_t lchild = VAREA_ENTRY(varea->node.node.avl_left);
        rt_varea_t rchild = VAREA_ENTRY(varea->node.node.avl_right);
        depth--;
        if (lchild)
            _print_varea(lchild, depth);
        else
            printf("0x**** ");

        if (rchild)
            _print_varea(rchild, depth);
        else
            printf("0x**** ");
    }
}
static void _print_tree(rt_aspace_t aspace)
{
    rt_varea_t varea = VAREA_ENTRY(aspace->tree.tree.root_node);
    if (!varea)
        return ;

    for (size_t i = 0; i < aspace->tree.tree.root_node->height; i++) {
        _print_varea(varea, i);
        putchar('\n');
    }

    return ;
}

static int _is_bst(rt_varea_t varea)
{
    rt_varea_t lchild = VAREA_ENTRY(varea->node.node.avl_left);
    rt_varea_t rchild = VAREA_ENTRY(varea->node.node.avl_right);
    if (lchild)
    {
        RT_ASSERT(lchild->node.node.parent == &varea->node.node);
        RT_ASSERT(varea->start > lchild->start);
    }
    if (rchild)
    {
        RT_ASSERT(rchild->node.node.parent == &varea->node.node);
        if (varea->start >= rchild->start)
        {
            RT_ASSERT(0);
        }
    }
    return 1;
}

/* return height of current varea */
static int _is_balanced(rt_varea_t varea)
{
    if (!varea)
    {
        return 1;
    }

    rt_varea_t lchild = VAREA_ENTRY(varea->node.node.avl_left);
    rt_varea_t rchild = VAREA_ENTRY(varea->node.node.avl_right);
    int lbal = _is_balanced(lchild);
    int rbal = _is_balanced(rchild);

    if (lbal && rbal)
    {
        int diff = lbal - rbal;
        if (diff > 1 || diff < -1)
        {
            printf("lbal %d, rbal %d\n", lbal, rbal);
            return 0;
        }
        else
        {
            int height = lbal > rbal ? lbal : rbal;
            return height + 1;
        }
    }
}

/* add bst assertion */
static int _check_asc_before(rt_varea_t varea, void *arg)
{
    if (varea->start >= _start && (!_boundary || varea->start >= _boundary) && _is_bst(varea))
    {
        _buf[_count] = varea;
        _start = varea->start;
        _boundary = varea->start + varea->size;
        _count++;
        RT_ASSERT(_count < BUF_SIZE);
    }
    else
    {
        RT_ASSERT(0);
    }
    return 0;
}

static int _check_asc_before_rev(rt_varea_t varea, void *arg)
{
    _count--;
    RT_ASSERT(varea == _buf[_count]);
    return 0;
}

static int _check_asc_after(rt_varea_t varea, void *arg)
{
    rt_varea_t add_elem = (rt_varea_t)arg;
    if (!_is_bst(varea))
    {
        RT_ASSERT(0);
    }

    if (varea == _buf[_count])
    {
        _buf[_count] = 0;
        _count++;
        RT_ASSERT(_count < BUF_SIZE);
    }
    else if (add_elem && add_elem == varea)
    {
        /* adding, skip adding elem */
    }
    else if (!add_elem && varea == _buf[_count + 1])
    {
        /* deleting */
        _buf[_count] = 0;
        _buf[_count] = 0;
        _count++;
        RT_ASSERT(_count < BUF_SIZE);
    }
    else
    {
        printf("add_elem %p, varea %p, _count %d, in buf %p\n",
            add_elem->start, varea->start, _count, _buf[_count]);
        RT_ASSERT(0);
    }
    return 0;
}

static int _aspace_traversal(rt_aspace_t aspace, int (*fn)(rt_varea_t varea, void *arg), void *arg)
{
    rt_varea_t varea = ASPACE_VAREA_FIRST(aspace);
    while (varea)
    {
        fn(varea, arg);
        varea = ASPACE_VAREA_NEXT(varea);
    }

    return 0;
}

static int _aspace_traversal_reverse(rt_aspace_t aspace, int (*fn)(rt_varea_t varea, void *arg), void *arg)
{
    rt_varea_t varea = ASPACE_VAREA_LAST(aspace);
    while (varea)
    {
        fn(varea, arg);
        varea = ASPACE_VAREA_PREV(varea);
    }

    return 0;
}

static int _check_bst_before(struct rt_aspace *aspace, struct rt_varea *varea)
{
    rt_varea_t root = VAREA_ENTRY(aspace->tree.tree.root_node);
    int height = _is_balanced(root);

    if (root)
        RT_ASSERT(height);

    memset(_buf, 0, sizeof(_buf)); // clear first avoiding none tree error
    _start = 0;
    _boundary = 0;
    _count = 0;

    _aspace_traversal(aspace, _check_asc_before, varea);
    int saved = _count;
    _aspace_traversal_reverse(aspace, _check_asc_before_rev, varea);
    _count = saved;

    return 1;
}

static int _check_bst_after(struct rt_aspace *aspace, struct rt_varea *varea, int isdel)
{
    rt_varea_t root = VAREA_ENTRY(aspace->tree.tree.root_node);
    int height = _is_balanced(root);

    if (root)
        RT_ASSERT(height);

    int prev_count = _count;
    _start = 0;
    _boundary = 0;
    _count = 0;
    _aspace_traversal(aspace, _check_asc_after, isdel ? NULL : varea);
    _count = isdel ? _count : _count + 1;

    if (isdel)
    {
        RT_ASSERT(prev_count - 1 == _count);
    }
    else
    {
        RT_ASSERT(prev_count + 1 == _count);
    }

    return 1;
}

#endif
