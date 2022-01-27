
#include <assert.h>
#include <functional>

// 斜堆
template <class T, class Compare = std::less<T>>
class SkewHeap
{
public:
    SkewHeap(): root_(nullptr) {}
    ~SkewHeap() { clear(); }

    // 拷贝构造函数
    SkewHeap(const SkewHeap& rhs)
    { root_ = clone(rhs.root_); }

    // 移动构造函数
    SkewHeap(SkewHeap&& rhs) noexcept: root_(rhs.root_)
    { rhs.root_ = nullptr; }

    // 拷贝赋值运算符
    SkewHeap& operator=(const SkewHeap& rhs)
    { return *this = SkewHeap(rhs); }

    // 移动赋值运算符
    SkewHeap& operator=(SkewHeap&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            rhs.root_ = nullptr;
        }
        return *this;
    }

    // 压入
    void push(const T& data)
    { root_ = _merge(root_, new Node(data)); }

    void push(T&& data)
    { root_ = _merge(root_, new Node(std::move(data))); }

    // 弹出
    void pop()
    {
        Node* newRoot = _merge(root_->left, root_->right);
        delete root_;
        root_ = newRoot;
    }

    // 合并
    void merge(SkewHeap& rhs) { merge(std::move(rhs)); }

    void merge(SkewHeap&& rhs)
    {
        if (this != &rhs)
        {
            root_ = _merge(root_, rhs.root_);
            rhs.root_ = nullptr;
        }
    }

    void clear() { destroy(root_); }

    const T& top() const { assert(!empty()); return root_->data; }
    bool empty() const { return root_ == nullptr; }

private:
    struct Node;

    static Node* _merge(Node* root1, Node* root2)
    {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;

        if (comp(root2->data, root1->data))
        {
            std::swap(root1, root2);
        }
        if (root1->left == nullptr)
        {
            root1->left = root2;
            return root1;
        }
        root1->right = _merge(root1->right, root2);
        //合并后直接交换
        std::swap(root1->left, root1->right);
        return root1;
    }

    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    static Node* clone(Node* node)
    {
        if (node == nullptr) return nullptr;
        return new Node(node->data, clone(node->left), clone(node->right));
    }

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    // 定义节点
    struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(const T& _data, Node* _left = nullptr, Node* _right = nullptr)
            : data(_data), left(_left), right(_right) {}

        Node(T&& _data, Node* _left = nullptr, Node* _right = nullptr)
            : data(std::move(_data)), left(_left), right(_right) {}
    };

    Node* root_; // 根节点
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

    SkewHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.push(x);

    SkewHeap<int, greater<int>> heap2;
    for (const auto& x : vec2) heap2.push(x);

    heap1.merge(heap2);

    while (!heap1.empty())
    {
        cout << heap1.top() << " ";
        heap1.pop();
    }
    cout << endl;
    return 0;
}
