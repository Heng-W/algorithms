
#include <assert.h>
#include <functional>
#include <vector>

// 二项堆
template <class T, class Compare = std::less<T>>
class BinomialHeap
{
public:
    BinomialHeap(): nodeCount_(0) {}
    ~BinomialHeap() { clear(); }

    // 拷贝构造函数
    BinomialHeap(const BinomialHeap& rhs)
        : roots_(rhs.roots_.size()),
          nodeCount_(rhs.nodeCount_)
    {
        for (int i = 0; i < (int)rhs.roots_.size(); ++i)
        {
            roots_[i] = clone(rhs.roots_[i]);
        }
    }

    // 移动构造函数
    BinomialHeap(BinomialHeap&& rhs) noexcept
        : roots_(std::move(rhs.roots_)),
          nodeCount_(rhs.nodeCount_)
    { rhs.nodeCount_ = 0; }

    // 拷贝赋值运算符
    BinomialHeap& operator=(const BinomialHeap& rhs)
    {
        BinomialHeap(rhs).swap(*this);
        return *this;
    }

    // 移动赋值运算符
    BinomialHeap& operator=(BinomialHeap&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    // 交换
    void swap(BinomialHeap& rhs) noexcept
    {
        using std::swap;
        roots_.swap(rhs.roots_);
        swap(nodeCount_, rhs.nodeCount_);
    }

    // 压入
    void push(const T& data) { merge(data); }
    void push(T&& data) { merge(std::move(data)); }

    // 弹出
    void pop()
    {
        int pos = findMinIndex();
        Node* child = roots_[pos]->child;
        delete roots_[pos];
        roots_[pos] = nullptr;

        BinomialHeap heap;
        heap.roots_.resize(pos); // pos等于度数
        // 反转
        for (int i = pos - 1; i >= 0; --i)
        {
            heap.roots_[i] = child;
            child = child->next;
        }
        heap.nodeCount_ = (1 << pos) - 1;
        nodeCount_ -= heap.nodeCount_ + 1;
        // 将heap合并到当前
        merge(heap);
    }

    // 合并
    void merge(BinomialHeap& rhs) { merge(std::move(rhs)); }

    void merge(BinomialHeap&& rhs)
    {
        if (this == &rhs) return;

        if (rhs.roots_.size() > roots_.size())
        {
            roots_.resize(rhs.roots_.size());
        }
        Node* carry = nullptr; // 进位
        for (int i = 0; i < (int)roots_.size(); ++i)
        {
            Node* root1 = roots_[i];
            Node* root2 =  i < (int)rhs.roots_.size() ? rhs.roots_[i] : nullptr;
            int caseNum = (root1 ? 1 : 0) | (root2 ? 2 : 0) | (carry ? 4 : 0);
            switch (caseNum)
            {
                case 0: // none
                case 1: // only this
                    break;
                case 2: // only rhs
                    roots_[i] = root2;
                    rhs.roots_[i] = nullptr;
                    break;
                case 4: // only carry
                    roots_[i] = carry;
                    carry = nullptr;
                    break;
                case 3: // this and rhs
                    carry = _merge(root1, root2);
                    roots_[i] = rhs.roots_[i] = nullptr;
                    break;
                case 5: // this and carry
                    carry = _merge(root1, carry);
                    roots_[i] = nullptr;
                    break;
                case 6: // rhs and carry
                    carry = _merge(root2, carry);
                    rhs.roots_[i] = nullptr;
                    break;
                case 7: // this, rhs and carry
                    roots_[i] = carry;
                    carry = _merge(root1, root2);
                    rhs.roots_[i] = nullptr;
                    break;
            }
        }
        // 最后有carry
        if (carry) roots_.push_back(carry);

        nodeCount_ += rhs.nodeCount_;
        // 清空rhs
        rhs.roots_.clear();
        rhs.nodeCount_ = 0;
    }

    void clear()
    {
        for (auto& root : roots_) destroy(root);
        nodeCount_ = 0;
    }

    const T& top() const { return roots_[findMinIndex()]->data; }

    bool empty() const { return nodeCount_ == 0; }
    int size() const { return nodeCount_; }

private:
    struct Node;

    // 构造包含一个元素的堆
    BinomialHeap(const T& data): nodeCount_(1)
    { roots_.push_back(new Node(data)); }

    BinomialHeap(T&& data): nodeCount_(1)
    { roots_.push_back(new Node(std::move(data))); }

    // 合并两棵树
    static Node* _merge(Node* root1, Node* root2)
    {
        if (comp(root2->data, root1->data))
        {
            std::swap(root1, root2);
        }
        root2->next = root1->child;
        root1->child = root2;
        return root1;
    }

    // 找到根节点值最小的树
    int findMinIndex() const
    {
        assert(!empty());
        int pos = 0;
        while (!roots_[pos]) ++pos;
        for (int i = pos + 1; i < (int)roots_.size(); ++i)
        {
            if (roots_[i] && comp(roots_[i]->data, roots_[pos]->data))
            {
                pos = i;
            }
        }
        return pos;
    }

    // 销毁
    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            destroy(node->child);
            destroy(node->next);
            delete node;
            node = nullptr;
        }
    }

    // 克隆
    static Node* clone(Node* node)
    {
        if (node == nullptr) return nullptr;
        return new Node(node->data, clone(node->child), clone(node->next));
    }

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    // 定义节点
    struct Node
    {
        T data;
        Node* child; // 第一个子节点
        Node* next; // 右兄弟

        Node(const T& _data, Node* _child = nullptr, Node* _next = nullptr)
            : data(_data), child(_child), next(_next) {}

        Node(T&& _data, Node* _child = nullptr, Node* _next = nullptr)
            : data(std::move(_data)), child(_child), next(_next) {}
    };

    std::vector<Node*> roots_; // 二项树
    int nodeCount_; // 节点数量
};


// 测试
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec1, vec2;
    for (int i = 0; i < 10; ++i)
    {
        vec1.push_back(rand() % 100);
        vec2.push_back(rand() % 100);
    }
    copy(vec1.cbegin(), vec1.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;
    copy(vec2.cbegin(), vec2.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    BinomialHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.push(x);

    BinomialHeap<int, greater<int>> heap2;
    for (const auto& x : vec2) heap2.push(x);

    heap1.merge(heap2);

    auto heap3 = heap1;

    while (!heap1.empty())
    {
        cout << heap1.top() << " ";
        heap1.pop();
    }
    cout << endl;

    while (!heap3.empty())
    {
        cout << heap3.top() << " ";
        heap3.pop();
    }
    cout << endl;
    return 0;
}
