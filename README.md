# Introduction

本目录下的 avl 实现是结合原有 rt-smart lwp_avl 和 parent avl 算法的实现。主要目标是提供一套可适用性高，兼容性好的平衡二叉树实现。设计上通过使用内嵌数据结构和新增父节点辅助索引加速性能。除了部分代码更新，也对原来 lwp_avl 的代码进行了整理，提高可读性。新 avl 实现的对外 API 仿照 Linux rb-tree 设计，以模板的形式导出。因此可以很方便地把原来使用 Linux rb-tree 的代码兼容到 rt-smart 中，免除开源协议不兼容的麻烦。

## Build

for AVL:

```bash
gcc -o build/avl_test -O2 -D_TEST_AVL benchmark/main.c benchmark/avl_adpt.c template/avl.c -I"./template" -I"./benchmark"
```

for AVL DEBUGGING:

```bash
gcc -o build/avl_test -DENABLE_DEBUG benchmark/main.c benchmark/avl_adpt.c template/avl.c -I"./template" -I"./benchmark"
```

for Linux rb-tree:

```bash
gcc -o build/rb_test -O2 -D_TEST_LINUXRB benchmark/main.c benchmark/rbtree_adpt.c template/linux-rbtree.c -I"./template" -I"./benchmark"
```

## Test

```bash
root@a9025fd90fd4:/home/c-library# build/avl_test 100000
Benchmark
looping times: 100000
Insertion: 28 ms
Search: 12 ms
Delete: 12 ms
Benchmark exit
```

```bash
root@a9025fd90fd4:/home/c-library# build/rb_test 100000
Benchmark
looping times: 100000
Insertion: 24 ms
Search: 16 ms
Delete: 12 ms
Benchmark exit
```

### 千万级数据集测试

测试基于 Linux Server 用户态进行，具有一定的随机性。
在完成数据插入后，不进行数据随机洗牌：

```bash
root@a9025fd90fd4:/home/c-library# build/avl_test 10000000
Benchmark
looping times: 10000000
Insertion: 2396 ms
Search: 2076 ms
Delete: 1976 ms
Benchmark exit
root@a9025fd90fd4:/home/c-library# build/rb_test 10000000
Benchmark
looping times: 10000000
Insertion: 2340 ms
Search: 2164 ms
Delete: 1856 ms
Benchmark exit
```

数据洗牌后，benchmark 结果如下：

```bash
root@a9025fd90fd4:/home/c-library# build/avl_test 10000000
Benchmark
looping times: 10000000
Insertion: 2332 ms
Search: 11527 ms
Delete: 12360 ms
Benchmark exit
root@a9025fd90fd4:/home/c-library# build/rb_test 10000000
Benchmark
looping times: 10000000
Insertion: 2176 ms
Search: 11755 ms
Delete: 12156 ms
Benchmark exit
```
