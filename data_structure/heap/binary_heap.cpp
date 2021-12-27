
#include <assert.h>
#include <functional>
#include <vector>

// 二叉堆
template <class T, class Compare = std::less<T>>
class BinaryHeap
{
public:
    using Sequence = std::vector<T>;

    BinaryHeap() = default;
    BinaryHeap(const Sequence& data): data_(data) { build(); }
    BinaryHeap(Sequence&& data): data_(std::move(data)) { build(); }

    // 压入
    void push(const T& x) { _push(x); }
    void push(T&& x) { _push(std::move(x)); }

    // 弹出
    void pop()
    {
        assert(!empty());
        data_.front() = std::move(data_.back());
        data_.pop_back();
        if (!empty()) percolateDown(0);
    }

    const T& top() const { assert(!empty()); return data_.front(); }

    void clear() { Sequence().swap(data_); }
    bool empty() const { return data_.empty(); }
    int size() const { return data_.size(); }

private:

    template <class X>
    void _push(X&& x)
    {
        data_.push_back(std::forward<X>(x));
        percolateUp(data_.size() - 1);
    }

    // 构建堆
    void build()
    {
        // 从最后一个非叶节点开始
        for (int i = data_.size() / 2 - 1; i >= 0; --i)
        {
            percolateDown(i);
        }
    }

    // 上溯
    void percolateUp(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        int parent = (holeIndex - 1) / 2;
        while (holeIndex > 0 && comp(value, data_[parent]))
        {
            data_[holeIndex] = std::move(data_[parent]);
            holeIndex = parent;
            parent = (holeIndex - 1) / 2;
        }
        data_[holeIndex] = std::move(value);
    }

    // 下溯
    void percolateDown(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        // 从左节点开始更新
        for (int i = holeIndex * 2 + 1; i < (int)data_.size(); i = i * 2 + 1)
        {
            // 指向较小的子节点
            if (i + 1 < (int)data_.size() && comp(data_[i + 1], data_[i]))
                ++i;
            if (comp(data_[i], value))
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

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    Sequence data_;
};


// 测试
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