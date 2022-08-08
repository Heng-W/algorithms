// 堆排序

#include <algorithm>

// 递归调整
template <class T>
void adjustHeapByRecursion(T* arr, int size, int idx)
{
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int maxIdx = idx;
    // 找到3个数中最大数的下标
    if (left < size && arr[left] > arr[maxIdx]) maxIdx = left;
    if (right < size && arr[right] > arr[maxIdx]) maxIdx = right;
    if (maxIdx != idx)
    {
        using std::swap;
        swap(arr[maxIdx], arr[idx]);
        adjustHeapByRecursion(arr, size, maxIdx); // 尾递归继续调整
    }
}


// 迭代调整
template <class T>
void adjustHeap(T* arr, int size, int idx)
{
    T value = std::move(arr[idx]);
    // 从左节点开始更新
    for (int i = idx * 2 + 1; i < size; i = i * 2 + 1)
    {
        // 指向较大的子节点
        if (i + 1 < size && arr[i + 1] > arr[i]) ++i;
        
        // 判断子节点大于父节点
        if (arr[i] > value)
        {
            arr[idx] = std::move(arr[i]);
            idx = i;
        }
        else
        {
            break;
        }
    }
    arr[idx] = std::move(value); // 将tmp值放到最终的位置
}


template <class T>
void heapSort(T* arr, int size)
{
    // 从最后一个非叶子节点开始，进行堆调整
    for (int i = size / 2 - 1; i >= 0; --i)
    {
        adjustHeap(arr, size, i);
    }
    using std::swap;
    for (int i = size - 1; i > 0; --i)
    {
        swap(arr[0], arr[i]);  // 将最大值交换到末尾保存
        adjustHeap(arr, i, 0); // 重新进行堆调整
    }
}


#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>

int main()
{
    using namespace std;
    srand(time(nullptr));
    vector<int> vec;
    for (int i = 0; i < 10; ++i)
    {
        vec.push_back(rand() % 100);
    }
    heapSort(&vec[0], vec.size());
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
