
#include <assert.h>
#include <functional>
#include <vector>
#include <queue> // for print heap
#include <iostream> // for std::cout

// 斐波那契堆
template <class T, class Compare = std::less<T>>
class FibonacciHeap
{
public:
    FibonacciHeap(): root_(nullptr), nodeCount_(0) {}
    ~FibonacciHeap() { clear(); }

    // 拷贝构造函数
    FibonacciHeap(const FibonacciHeap& rhs)
        : root_(nullptr), nodeCount_(rhs.nodeCount_)
    {
        if (rhs.root_)
        {
            Node* cur = rhs.root_;
            Node* copy = new Node(cur->data);
            root_ = copy;
            while (cur->next != rhs.root_)
            {
                copy->next = new Node(cur->next->data);
                copy->next->prev = copy;
                cur = cur->next;
                copy = copy->next;
                Node* first, *prev;
                copy->child = clone(cur->child, copy, first, prev, 0);
            }
            copy->next = root_;
            root_->prev = copy;
        }
    }

    // 移动构造函数
    FibonacciHeap(FibonacciHeap&& rhs) noexcept
        : root_(rhs.root_), nodeCount_(rhs.nodeCount_)
    {
        rhs.root_ = nullptr;
        rhs.nodeCount_ = 0;
    }

    // 拷贝赋值运算符
    FibonacciHeap& operator=(const FibonacciHeap& rhs)
    {
        FibonacciHeap copy = rhs;
        return *this = std::move(copy);
    }

    // 移动赋值运算符
    FibonacciHeap& operator=(FibonacciHeap&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            nodeCount_ = rhs.nodeCount_;
            rhs.root_ = nullptr;
            rhs.nodeCount_ = 0;
        }
        return *this;
    }

    // 压入
    void push(const T& data) { merge(data); }
    void push(T&& data) { merge(std::move(data)); }

    // 弹出
    void pop()
    {
        assert(!empty());
        // 子节点添加进根链表
        Node* begin; // 指向合并后的根链表
        if (root_->next == root_) // 根链表只有root节点
        {
            begin = root_->child;
        }
        else
        {
            begin = root_->next;
            root_->prev->next = root_->next;
            root_->next->prev = root_->prev;
            if (root_->child)
                splice(begin, root_->child, root_->child->prev);
        }
        delete root_;
        root_ = nullptr;
        --nodeCount_;

        if (begin == nullptr) return; // 堆已经为空

        std::vector<Node*> roots; // 合并得到的不同度数的树
        Node* cur = begin;
        while (true)
        {
            Node* next = cur->next; // 保存next节点
            cur->prev = cur->next = cur; // 断开节点
            if (cur->degree >= (int)roots.size())
                roots.resize(cur->degree + 1);
            // 合并度数相同的树
            while (roots[cur->degree])
            {
                Node* root = roots[cur->degree];
                roots[cur->degree] = nullptr;
                if (comp(cur->data, root->data)) // 使root小于cur
                {
                    std::swap(cur, root);
                }
                //cur合并到root
                if (root->child == nullptr)
                    root->child = cur;
                else
                    splice(root->child, cur, cur);

                ++root->degree;
                cur = root; // 继续迭代
                if (cur->degree >= (int)roots.size())
                    roots.resize(cur->degree + 1);
            }
            roots[cur->degree] = cur;
            cur = next;
            if (cur == begin) break;
        }
        // 合并串联到根链表上
        for (int i = 0; i < (int)roots.size(); ++i)
        {
            if (roots[i]) root_ = _merge(root_, roots[i]);
        }
    }

    // 合并
    void merge(FibonacciHeap& rhs) { merge(std::move(rhs)); }

    void merge(FibonacciHeap&& rhs)
    {
        if (this == &rhs) return;
        root_ = _merge(root_, rhs.root_);
        nodeCount_ += rhs.nodeCount_;

        rhs.root_ = nullptr;
        rhs.nodeCount_ = 0;
    }

    // 清除
    void clear()
    {
        destroy(root_);
        nodeCount_ = 0;
    }

    // 打印
    void print() const
    {
        std::queue<Node*> que;
        Node* cur = root_;
        while (true)
        {
            que.push(cur);
            cur = cur->next;
            if (cur == root_) break;
        }
        while (!que.empty())
        {
            Node* cur = que.front();
            que.pop();
            std::cout << cur->data << " ";
            Node* child = cur->child;
            for (int i = 0; i < cur->degree; ++i)
            {
                que.push(child);
                child = child->next;
            }
        }
    }

    const T& top() const { assert(!empty()); return root_->data; }

    bool empty() const { return root_ == nullptr; }

private:
    struct Node;

    // 构造包含一个元素的堆
    FibonacciHeap(const T& data): nodeCount_(1)
    { root_ = new Node(data); }

    FibonacciHeap(T&& data): nodeCount_(1)
    { root_ = new Node(std::move(data)); }

    // 合并串联到根链表上
    Node* _merge(Node* root1, Node* root2)
    {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;

        if (comp(root2->data, root1->data))
        {
            std::swap(root1, root2);
        }
        splice(root1, root2, root2->prev);
        return root1;
    }

    // [first,last]内元素接合于pos位置之前
    void splice(Node* pos, Node* first, Node* last)
    {
        pos->prev->next = first;
        first->prev = pos->prev;
        last->next = pos;
        pos->prev = last;
    }

    // 销毁
    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            Node* cur = node->child;
            for (int i = 0; i < node->degree; ++i)
            {
                Node* next = cur->next;
                destroy(cur);
                cur = next;
            }
            delete node;
            node = nullptr;
        }
    }

    // 克隆
    static Node* clone(Node* node, Node* parent,
                       Node*& first, Node*& prev, int count)
    {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->data);
        copy->degree = node->degree;
        if (count == 0) first = prev = copy;
        copy->prev = prev;
        if (++count == parent->degree)
        {
            copy->next = first;
            first->prev = copy;
            return first;
        }
        copy->next = clone(node->next, parent, first, copy, count);
        copy->child = clone(node->child, copy, first, prev, 0);
        return copy;
    }

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    // 定义节点
    struct Node
    {
        T data;
        int degree = 0;
        Node* child = nullptr; //第一个子节点
        Node* next; //兄弟（后继）
        Node* prev; //兄弟（前驱）

        Node(const T& _data): data(_data) { prev = next = this; }
        Node(T&& _data): data(std::move(_data)) { prev = next = this; }
    };

    Node* root_; // 根节点，指向根链表上最小元素
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

    FibonacciHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.push(x);

    FibonacciHeap<int, greater<int>> heap2;
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
