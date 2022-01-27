
#include <assert.h>
#include <memory>

// 链表实现的队列
template <class T>
class Queue
{
public:
    Queue(): head_(nullptr), tail_(nullptr), size_(0) {}
    ~Queue() { clear(); }

    // 拷贝构造函数
    Queue(const Queue& rhs)
        : Queue()
    {
        Node* src = rhs.head_;
        while (src)
        {
            push(src->data);
            src = src->next;
        }
    }

    // 移动构造函数
    Queue(Queue&& rhs) noexcept
        : head_(rhs.head_), tail_(rhs.tail_), size_(rhs.size)
    {
        rhs.head_ = rhs.tail_ = nullptr;
        rhs.size_ = 0;
    }

    // 拷贝赋值运算符
    Queue& operator=(const Queue& rhs)
    {
        return *this = std::move(Query(rhs));
    }

    // 移动赋值运算符
    Queue& operator=(Queue&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            head_ = rhs.head_;
            tail_ = rhs.tail_;
            size_ = rhs.size_;
            // 清空rhs
            rhs.head_ = rhs.tail_ = nullptr;
            rhs.size_ = 0;
        }
        return *this;
    }

    // 压入
    void push(const T& data) { _push(data); }
    void push(T&& data) { _push(std::move(data)); }

    // 弹出
    void pop()
    {
        assert(!empty());
        Node* tmp = head_;
        head_ = head_->next;
        delete tmp;
        --size_;
    }

    void clear()
    {
        Node* cur = head_;
        while (cur)
        {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        head_ = tail_ = nullptr;
        size_ = 0;
    }

    const T& front() const { return head_->data; }

    bool empty() const { return size_ == 0; }
    int size() const { return size_; }

private:

    template <class X>
    void _push(X&& x)
    {
        Node* node = new Node(std::forward<X>(x));
        node->next = nullptr;
        if (empty())
        {
            head_ = tail_ = node;
        }
        else
        {
            tail_->next = node;
            tail_ = node;
        }
        ++size_;
    }

    // 节点
    struct Node
    {
        T data;
        Node* next;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* head_;
    Node* tail_;
    int size_; // 元素数量
};


// 测试
#include <ctime>
#include <cstdlib>
#include <iostream>

int main()
{
    using namespace std;
    srand(time(nullptr));

    Queue<int> q;
    for (int i = 0; i < 5; ++i)
    {
        int data = rand() % 100;
        q.push(data);
        cout << data << " enQueue" << endl;
    }
    while (!q.empty())
    {
        cout << q.front() << " deQueue" << endl;
        q.pop();
    }
    return 0;
}
