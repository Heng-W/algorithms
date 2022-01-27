
#include <algorithm>

// 选择排序
template <class T>
void selectSort(T* arr, int size)
{
    if (arr == nullptr || size <= 1) return;
    for (int i = 0; i < size - 1; ++i)
    {
        int min = i;
        // 找到最小值下标
        for (int j = i + 1; j < size; ++j)
        {
            if (arr[j] < arr[min])
            {
                min = j;
            }
        }
        if (min != i)
        {
            using std::swap;
            swap(arr[i], arr[min]);
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
    selectSort(&vec[0], vec.size());
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
