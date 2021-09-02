//基数排序
#include <vector>
#include <algorithm>

void radixSort(std::vector<int>& data)
{
    int max = *std::max_element(data.cbegin(), data.cend());
    std::vector<int> tmp(data.size());
    int buckets[10];

    for (int exp = 1; exp <= max; exp *= 10)
    {
        std::fill_n(buckets, 10, 0);
        //记录数据出现次数
        for (int i = 0; i < data.size(); ++i)
            ++buckets[data[i] / exp % 10];
        //更改buckets使其为位置索引
        for (int i = 1; i < 10; ++i)
            buckets[i] += buckets[i - 1];
        //按照当前数位进行排序，结果存储在tmp数组
        for (int i = data.size() - 1; i >= 0; --i)
            tmp[--buckets[data[i] / exp % 10]] = data[i];
        //指向tmp存储的数据
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
        vec.push_back(rand() % 1000);
    }
    radixSort(vec);
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}

