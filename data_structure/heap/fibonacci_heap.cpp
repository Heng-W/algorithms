
#include <assert.h>
#include <functional>
#include <vector>
#include <queue>
#include <iostream>

//斐波那契堆
template <class T, class Compare = std::less<T>>
class FibonacciHeap
{
public:
    FibonacciHeap(): root_(nullptr), nodeCount_(0) {}
    ~FibonacciHeap() { clear(); }

    FibonacciHeap(FibonacciHeap&& rhs) noexcept
        : root_(rhs.root_), nodeCount_(rhs.nodeCount_)
    {
        rhs.root_ = nullptr;
        rhs.nodeCount_ = 0;
    }

    FibonacciHeap& operator=(FibonacciHeap&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            std::swap(*this, rhs);
        }
        return *this;
    }

    void insert(const T& data)
    {
        Node* newNode = new Node(data);
        newNode->prev = newNode->next = newNode;
        root_ = merge(root_, newNode);
        ++nodeCount_;
    }

    void insert(T&& data)
    {
        Node* newNode = new Node(std::move(data));
        newNode->prev = newNode->next = newNode;
        root_ = merge(root_, newNode);
        ++nodeCount_;
    }

    void pop()
    {
        assert(root_ != nullptr);
        //从根链表中移除最小节点
        root_->prev->next = root_->next;
        root_->next->prev = root_->prev;

        Node* begin = root_->next;
        Node* child = root_->child;

        delete root_;
        root_ = nullptr;
        if (--nodeCount_ == 0) return;

        //子节点添加进根链表
        if (child) splice(begin, child, child->prev);

        std::vector<Node*> roots;
        Node* cur = begin;
        while (true)
        {
            Node* next = cur->next;
            cur->prev = cur->next = cur;
            if (cur->degree >= roots.size())
                roots.resize(cur->degree + 1);
            while (roots[cur->degree])
            {
                Node* root = roots[cur->degree];
                roots[cur->degree] = nullptr;
                if (comp(cur->data, root->data))
                {
                    std::swap(cur, root);
                }
                //cur合并到root
                if (root->child == nullptr)
                    root->child = cur;
                else
                    splice(root->child, cur, cur);
                    
                ++root->degree;
                cur = root;//继续迭代
                if (cur->degree >= roots.size())
                    roots.resize(cur->degree + 1);
            }
            roots[cur->degree] = cur;
            cur = next;
            if (cur == begin) break;
        }
        for (int i = 0; i < roots.size(); ++i)
        {
            if (roots[i]) root_ = merge(root_, roots[i]);
        }
    }

    void merge(FibonacciHeap& rhs) { merge(std::move(rhs)); }

    void merge(FibonacciHeap&& rhs)
    {
        if (this == &rhs)
            return;
        root_ = merge(root_, rhs.root_);
        nodeCount_ += rhs.nodeCount_;

        rhs.root_ = nullptr;
        rhs.nodeCount_ = 0;
    }

    void clear()
    {
        destroy(root_);
        nodeCount_ = 0;
    }

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

    const T& top() const { assert(root_ != nullptr); return root_->data; }

    bool empty() const { return nodeCount_ == 0; }

private:

    struct Node;

    Node* merge(Node* root1, Node* root2)
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

    //[first,last]内元素接合于pos位置之前
    void splice(Node* pos, Node* first, Node* last)
    {
        pos->prev->next = first;
        first->prev = pos->prev;
        last->next = pos;
        pos->prev = last;
    }

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

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    struct Node
    {
        T data;
        int degree = 0;
        Node* child = nullptr; //第一个子节点
        Node* next; //兄弟（后继）
        Node* prev; //兄弟（前驱）

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* root_;
    int nodeCount_;
};


//测试
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
    for (const auto& x : vec1) heap1.insert(x);

    FibonacciHeap<int, greater<int>> heap2;
    for (const auto& x : vec2) heap2.insert(x);

    heap1.merge(heap2);

    while (!heap1.empty())
    {
        cout << heap1.top() << " ";
        heap1.pop();
    }
    cout << endl;
    return 0;
}
