
#include <assert.h>
#include <functional>
#include <vector>

// d堆
template <class T, int D, class Compare = std::less<T>>
class DaryHeap
{
public:
    using Sequence = std::vector<T>;

    DaryHeap() = default;
    DaryHeap(const Sequence& data): data_(data) { build(); }
    DaryHeap(Sequence&& data): data_(std::move(data)) { build(); }

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
        for (int i = (data_.size() - 2) / D; i >= 0; --i)
        {
            percolateDown(i);
        }
    }

    // 上溯
    void percolateUp(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        int parent = (holeIndex - 1) / D;
        while (holeIndex > 0 && comp(value, data_[parent]))
        {
            data_[holeIndex] = std::move(data_[parent]);
            holeIndex = parent;
            parent = (holeIndex - 1) / D;
        }
        data_[holeIndex] = std::move(value);
    }

    // 下溯
    void percolateDown(int holeIndex)
    {
        T value = std::move(data_[holeIndex]);
        // 从左节点开始更新
        for (int i = holeIndex * D + 1; i < (int)data_.size(); i = i * D + 1)
        {
            // 指向较小的子节点
            int minPos = i;
            int end = std::min(i + D, (int)data_.size());
            for (int k = i + 1; k < end; ++k)
            {
                if (comp(data_[k], data_[minPos])) minPos = k;
            }
            i = minPos;
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

    Sequence data_; // 数据
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
    for (int i = 0; i < 15; ++i)
    {
        vec.push_back(rand() % 100);
    }
    copy(vec.cbegin(), vec.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;
    DaryHeap<int, 4, greater<int>> heap(std::move(vec));
    while (!heap.empty())
    {
        cout << heap.top() << " ";
        heap.pop();
    }
    cout << endl;
    return 0;
}
