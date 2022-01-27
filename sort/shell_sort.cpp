
#include <algorithm>

// 希尔排序
template <class T>
void shellSort(T* arr, int size)
{
    if (arr == nullptr || size <= 1) return;
    int pos;
    for (int gap = size / 2; gap > 0; gap /= 2)
    {
        for (int i = gap; i < size; ++i)
        {
            T tmp = std::move(arr[i]);
            for (pos = i; pos >= gap && arr[pos - gap] > tmp; pos -= gap)
            {
                arr[pos] = arr[pos - gap];
            }
            arr[pos] = std::move(tmp);
        }
    }
}


// 测试
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
    shellSort(&vec[0], vec.size());
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
