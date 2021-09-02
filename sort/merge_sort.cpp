//归并排序

#include <vector>

//合并两个有序序列
template <class T>
void merge(const T* first1, const T* last1,
           const T* first2, const T* last2,
           T* result)
{
    while (first1 != last1 && first2 != last2)
    {
        if (*first1 <= *first2)
            *result++ = *first1++;
        else
            *result++ = *first2++;
    }
    while (first1 != last1)
        *result++ = *first1++;
    while (first2 != last2)
        *result++ = *first2++;
}

//归并排序（递归）
template <class T>
void mergeSort(T* first, T* last)
{
    if (last - first > 1)
    {
        T* mid = first + (last - first) / 2;
        mergeSort(first, mid);
        mergeSort(mid, last);
        std::vector<T> tmp(first, mid);
        merge(&*tmp.cbegin(), &*tmp.cend(), mid, last, first);
    }
}

//归并排序（递归），优化拷贝
template <class T>
void mergeSort2(T* arr, T* tmp, int size)
{
    if (size > 1)
    {
        int half = size / 2;
        mergeSort2(tmp, arr, half);
        mergeSort2(tmp + half, arr + half, size - half);
        merge(tmp, tmp + half, tmp + half, tmp + size, arr);
    }
}

template <class T>
void mergeSort2(T* first, T* last)
{
    std::vector<T> tmp(first, last);
    mergeSort2(first, &tmp[0], last - first);
}


//归并排序（非递归）
template <class T>
void mergeSortI(std::vector<T>& data)
{
    std::vector<T> tmp = data;
    //len：子序列长度
    for (int len = 1; len < data.size(); len *= 2)
    {
        T* cur = &*data.begin();
        T* end = &*data.end();
        T* dst = &*tmp.begin();
        while (end - cur >= 2 * len)
        {
            merge(cur, cur + len, cur + len, cur + 2 * len, dst);
            cur += 2 * len;
            dst += 2 * len;
        }
        if (end - cur > len)
            merge(cur, cur + len, cur + len, end, dst);
        else
            std::copy(cur, end, dst);
        data.swap(tmp);
    }
}


#include <ctime>
#include <cstdlib>
#include <iostream>

int main()
{
    using namespace std;
    srand(time(nullptr));
    vector<int> vec;
    for (int i = 0; i < 11; ++i)
    {
        vec.push_back(rand() % 100);
    }
    //mergeSort2(&*vec.begin(), &*vec.end());
    mergeSortI(vec);
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
