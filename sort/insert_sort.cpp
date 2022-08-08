
#include <algorithm>

// 插入排序
template <class T>
void insertSort(T* arr, int size)
{
    if (arr == nullptr || size <= 1) return;
    for (int i = 1; i < size; ++i)
    {
        T tmp = std::move(arr[i]);
        int pos = i;
        while (pos > 0 && arr[pos - 1] > tmp)
        {
            arr[pos] = arr[pos - 1];
            --pos;
        }
        arr[pos] = std::move(tmp);
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
    insertSort(&vec[0], vec.size());
    for (const auto& x : vec) cout << x << " ";
    cout << endl;
    return 0;
}
