
#include <assert.h>
#include <functional>
#include <vector>

//二叉堆
template <class T, class Compare = std::less<T>>
class BinaryHeap
{
public:
    using Sequence = std::vector<T>;

    BinaryHeap() = default;

    BinaryHeap(const Sequence& data): data_(data) { build(); }
    BinaryHeap(Sequence&& data): data_(std::move(data)) { build(); }


    void push(const T& x) { _push(x); }
    void push(T&& x) { _push(std::move(x)); }


    void pop()
    {
        assert(!data_.empty());
        data_.front() = std::move(data_.back());
        data_.pop_back();
        if (!data_.empty()) percolateDown(0);
    }

    const T& top() const { return data_.front(); }

    void clear() { std::vector<T>().swap(data_); }

    bool empty() const { return data_.empty(); }

    int count() const { return data_.size(); }

private:

    template <class X>
    void _push(X&& x)
    {
        data_.push_back(std::forward<X>(x));
        percolateUp(data_.size() - 1);
    }

    void build()
    {
        //从最后一个非叶节点开始
        for (int i = data_.size() / 2 - 1; i >= 0; --i)
        {
            percolateDown(i);
        }
    }

    void percolateUp(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        int parent = (holeIndex - 1) / 2;
        while (holeIndex > 0 && comp_(value, data_[parent]))
        {
            data_[holeIndex] = std::move(data_[parent]);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        data_[holeIndex] = std::move(value);
    }

    void percolateDown(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        //从左节点开始更新
        for (int i = holeIndex * 2 + 1; i < data_.size(); i = i * 2 + 1)
        {
            //指向较小的子节点
            if (i + 1 < data_.size() && comp_(data_[i + 1], data_[i]))
                ++i;
            if (comp_(data_[i], value))
            {
                data_[holeIndex] = std::move(data_[i]);
                holeIndex = i;
            }
            else
            {
                break;
            }
        }
        data_[holeIndex] = std::move(value);
    }

    std::vector<T> data_;
    Compare comp_;
};


#include <ctime>
#include <cstdlib>
#include <iterator>
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
    copy(vec.cbegin(), vec.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;
    BinaryHeap<int, greater<int>> heap(std::move(vec));
    while (!heap.empty())
    {
        cout << heap.top() << " ";
        heap.pop();
    }
    cout << endl;
    return 0;
}