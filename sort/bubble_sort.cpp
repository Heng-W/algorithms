
#include <algorithm>

// 冒泡排序
template <class T>
void bubbleSort(T* arr, int size)
{
    if (arr == nullptr || size <= 1) return;
    for (int i = 0; i < size - 1; ++i)
    {
        for (int j = 0; j < size - i - 1; ++j)
        {
            if (arr[j] > arr[j + 1])
            {
                using std::swap;
                swap(arr[j], arr[j + 1]);
            }
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
    bubbleSort(&vec[0], vec.size());
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
