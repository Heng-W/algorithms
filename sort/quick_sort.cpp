// 快速排序
#include <iostream>


//三数取中
inline int midIndex(int* arr, int left, int right)
{
    int mid = left + (right - left) / 2;

    if (arr[left] < arr[mid])
    {
        if (arr[mid] < arr[right]) //left<mid<right
            return mid;
        else if (arr[left] < arr[right]) //left<mid, right<=mid, left<right
            return right;
        else
            return left;
    }
    else
    {
        if (arr[right] < arr[mid]) //right<mid<=left
            return mid;
        else if (arr[left] < arr[right]) //mid<=left, mid<=right, left<right
            return left;
        else
            return right;
    }
}


int partition(int* arr, int left, int right)
{
    std::swap(arr[left], arr[midIndex(arr, left, right)]);

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
    std::swap(arr[left], arr[midIndex(arr, left, right)]);

    int originLeft = left;
    int pivot = arr[left];

    while (true)
    {
        while (left < right && arr[right] >= pivot) --right;
        while (left < right && arr[left] <= pivot) ++left;
        if (left < right)
            std::swap(arr[left], arr[right]);
        else
            break;
    }
    arr[originLeft] = arr[left];
    arr[left] = pivot;
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


int main()
{
    int arr[] = {8, 1, 14, 3, 21, 5, 7, 10};
    quickSort(arr, 0, sizeof(arr) / sizeof(int) - 1);
    for (const auto& e : arr)
    {
        std::cout << e << std::endl;
    }
    return 0;
}
