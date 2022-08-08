// 快速排序

#include <algorithm>

// 三数取中
inline int midIndex(int* arr, int left, int right)
{
    int mid = left + (right - left) / 2;

    if (arr[left] < arr[mid])
    {
        if (arr[mid] < arr[right]) // left<mid<right
            return mid;
        else if (arr[left] < arr[right]) // left<mid, right<=mid, left<right
            return right;
        else
            return left;
    }
    else
    {
        if (arr[right] < arr[mid]) // right<mid<=left
            return mid;
        else if (arr[left] < arr[right]) // mid<=left, mid<=right, left<right
            return left;
        else
            return right;
    }
}


int partition(int* arr, int left, int right)
{
    using std::swap;
    swap(arr[left], arr[midIndex(arr, left, right)]);

    int pivot = arr[left];

    while (left < right)
    {
        while (left < right && arr[right] >= pivot) --right;
        arr[left] = arr[right];
        while (left < right && arr[left] <= pivot) ++left;
        arr[right] = arr[left];
    }
    arr[left] = pivot;
    return left;
}


int partition1(int* arr, int left, int right)
{
    using std::swap;
    swap(arr[left], arr[midIndex(arr, left, right)]);

    int pivot = arr[left];
    int pivotIndex = left;

    while (left < right)
    {
        while (left < right && arr[right] >= pivot) --right;
        while (left < right && arr[left] <= pivot) ++left;
        swap(arr[left], arr[right]);
    }
    swap(arr[pivotIndex], arr[left]);
    return left;
}


void quickSort(int* arr, int left, int right)
{
    if (left < right)
    {
        int cut = partition(arr, left, right);
        quickSort(arr, left, cut - 1);
        quickSort(arr, cut + 1, right);
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
    quickSort(&vec[0], 0, vec.size() - 1);
    for (const auto& x : vec)
        cout << x << " ";
    cout << endl;
    return 0;
}
