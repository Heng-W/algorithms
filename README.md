# algorithms
# 算法

### 实现语言：C++

### 构建方式：cmake
#### 例如在Linux下编译：
```shell
mkdir build 
cd build
cmake ..
make
```
### 文件说明
#### 数据结构（data_structure）

- array_list.cpp：动态数组实现的线性表
- linked_list.cpp：双向循环链表实现的线性表
- queue.cpp：链表实现的队列
- stack.cpp：链表实现的堆栈
- circular_queue.cpp：数组实现的环形队列
- binary_heap.cpp：二叉堆，实现优先级队列
- binary_tree.cpp：二叉树
- binary_search_tree.cpp：二叉搜索树
- avl_tree.cpp：avl树
- rb_tree.hpp：红黑树
- set.cpp：红黑树实现的集合
- map.cpp：红黑树实现的map
- hash_table.hpp：哈希表
- hash_set.cpp：哈希表实现的集合
- hash_map.cpp：哈希表实现的map
- linked_hash_table.hpp：HashTable与双向链表结合，可实现缓存机制
- linked_hash_map.cpp：HashMap与双向链表结合
- disjoint_set.cpp：并查集
- skip_list.cpp：跳跃表


#### 排序（sort）
- quick_sort.cpp：快速排序
- merge_sort.cpp：归并排序
- heap_sort.cpp：堆排序
- shell_sort.cpp：希尔排序
- radix_sort.cpp：基数排序
- insert_sort.cpp：插入排序
- select_sort.cpp：选择排序
- bubble_sort.cpp：冒泡排序


#### 数值计算（numerical）

- SOR.cpp：SOR迭代法（线性方程组求解）
- gaussian_elimination.cpp：列主元高斯消元法（线性方程组求解）
- cholesky.cpp：平方根法（线性方程组求解）

- cubic_spline_interpolation.cpp：三次样条插值法
- newton_interpolation.cpp：牛顿插值法
- romberg_integration.cpp：Romberg法求积分
