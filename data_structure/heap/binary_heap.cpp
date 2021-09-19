//二叉堆
#include <assert.h>
#include <vector>
#include <iostream>


template <class T, class Compare = std::less<T>>
class BinaryHeap
{
    friend std::ostream& operator<<(std::ostream& out, const BinaryHeap& heap)
    {
        for (const auto& x : heap.data_)
            out << x << " ";
        return out;
    }
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

    T& top() { return data_.front(); }
    const T& top() const { return data_.front(); }

    void clear() { std::vector<T>().swap(data_); }

    bool empty() const { return data_.empty(); }

    int size() const { return data_.size(); }

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

    void percolateUp(int holeIdx)
    {
        T value = std::move(data_[holeIdx]);
        int parent = (holeIdx - 1) / 2;
        while (holeIdx > 0 && comp_(data_[parent], value))
        {
            data_[holeIdx] = std::move(data_[parent]);
            holeIdx = parent;
            parent = (holeIdx - 1) / 2;
        }
        data_[holeIdx] = std::move(value);
    }

    void percolateDown(int holeIdx)
    {
        T value = std::move(data_[holeIdx]);
        //从左节点开始更新
        for (int i = holeIdx * 2 + 1; i < data_.size(); i = i * 2 + 1)
        {
            //指向较大的子节点
            if (i + 1 < data_.size() && comp_(data_[i], data_[i + 1]))
                ++i;
            if (comp_(value, data_[i]))
            {
                data_[holeIdx] = std::move(data_[i]);
                holeIdx = i;
            }
            else
            {
                break;
            }
        }
        data_[holeIdx] = std::move(value);
    }

    std::vector<T> data_;
    Compare comp_;
};


#include <ctime>
#include <cstdlib>

int main()
{
    using namespace std;
    srand(time(nullptr));
    vector<int> v;
    for (int i = 0; i < 10; ++i)
    {
        v.push_back(1000.0 * rand() / RAND_MAX);
    }
    BinaryHeap<int, std::greater<int>> heap(std::move(v));
    while (!heap.empty())
    {
        cout << heap.top() << " ";
        heap.pop();
    }
    cout << endl;
    return 0;
}
