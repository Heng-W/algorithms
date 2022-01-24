
#include <assert.h>
#include <functional>
#include <queue> // for pop heap 

// 配对堆
template <class T, class Compare = std::less<T>>
class PairingHeap
{
    struct Node;
public:
    PairingHeap(): root_(nullptr) {}
    ~PairingHeap() { clear(); }

    // 拷贝构造函数
    PairingHeap(const PairingHeap& rhs)
    { root_ = clone(rhs.root_); }

    // 移动构造函数
    PairingHeap(PairingHeap&& rhs): root_(rhs.root_)
    { rhs.root_ = nullptr; }

    // 拷贝赋值运算符
    PairingHeap& operator=(const PairingHeap& rhs)
    { return *this = PairingHeap(rhs); }

    // 移动赋值运算符
    PairingHeap& operator=(PairingHeap&& rhs)
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
    Node* push(const T& data)
    {
        Node* node = new Node(data);
        root_ = _merge(root_, node);
        return node;
    }
    Node* push(T&& data)
    {
        Node* node = new Node(std::move(data));
        root_ = _merge(root_, node);
        return node;
    }

    // 合并
    void merge(PairingHeap& rhs) { merge(std::move(rhs)); }

    void merge(PairingHeap&& rhs)
    {
        if (this != &rhs)
        {
            root_ = _merge(root_, rhs.root_);
            rhs.root_ = nullptr;
        }
    }

    // 弹出
    void pop()
    {
        assert(!empty());
        std::queue<Node*> que;
        for (Node* cur = root_->child; cur; cur = cur->next)
        {
            que.push(cur);
        }
        // 两两配对合并
        while (que.size() > 1)
        {
            Node* node1 = que.front();
            que.pop();
            Node* node2 = que.front();
            que.pop();
            que.push(_merge(node1, node2));
        }
        delete root_;
        if (que.empty())
        {
            root_ = nullptr;
            return;
        }
        root_ = que.front();
        root_->next = root_->prev = nullptr;
    }

    // 更新键值，若堆顶为min只能decreaseKey，为max只能increaseKey
    bool updateKey(Node* node, const T& newValue)
    {
        if (comp(node->data, newValue)) return false;

        node->data = newValue;
        if (node == root_) return true;

        if (node == node->prev->child)
            node->prev->child = node->next;
        else
            node->prev->next = node->next;

        if (node->next != nullptr) node->next->prev = node->prev;
        node->next = node->prev = nullptr;
        root_ = _merge(root_, node);
        return true;
    }

    const T& top() const { assert(!empty()); return root_->data; }

    void clear() { destroy(root_); }
    bool empty() const { return root_ == nullptr; }

private:

    // 合并两棵树
    static Node* _merge(Node* root1, Node* root2)
    {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;

        if (comp(root2->data, root1->data))
        {
            std::swap(root1, root2);
        }
        root2->next = root1->child;
        if (root2->next) root2->next->prev = root2;
        root1->child = root2;
        root2->prev = root1;
        return root1;
    }

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

    static Node* clone(Node* node, Node* prev = nullptr)
    {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->data);
        copy->prev = prev;
        copy->next = clone(node->next, copy);
        copy->child = clone(node->child, copy);
        return copy;
    }

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    // 定义节点
    struct Node
    {
        T data;
        Node* child = nullptr; // 第一个子节点
        Node* next = nullptr; // 右兄弟
        Node* prev = nullptr; // 前驱，指向左兄弟或父节点

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
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

    PairingHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.push(x);

    PairingHeap<int, greater<int>> heap2;
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


