
#include <assert.h>
#include <memory>
#include <iostream>


// 链表实现的堆栈
template <class T>
class Stack
{
public:
    Stack(): head_(nullptr), size_(0) {}
    ~Stack() { clear(); }

    // 拷贝构造函数
    Stack(const Stack& rhs)
        : head_(nullptr), size_(0)
    {
        Node* src = rhs.head_;
        while (src)
        {
            push(src->data);
            src = src->next;
        }
    }

    // 移动构造函数
    Stack(Stack&& rhs) noexcept
        : head_(rhs.head_), size_(rhs.size_)
    {
        rhs.head_ = nullptr;
        rhs.size_ = 0;
    }

    // 拷贝赋值运算符
    Stack& operator=(const Stack& rhs)
    {
        Stack copy = rhs;
        return *this = std::move(copy);
    }

    // 移动赋值运算符
    Stack& operator=(Stack&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            head_ = rhs.head_;
            size_ = rhs.size_;

            rhs.head_ = nullptr;
            rhs.size_ = 0;
        }
        return *this;
    }

    // 压入
    void push(const T& x) { _push(x); }
    void push(T&& x) { _push(std::move(x)); }

    // 弹出
    void pop()
    {
        assert(!empty());
        Node* next = head_->next;
        delete head_;
        head_ = next;
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
        size_ = 0;
    }

    const T& top() const { return head_->data; }

    bool empty() const { return size_ == 0; }
    int size() const { return size_; }

private:

    template <class X>
    void _push(X&& x)
    {
        Node* node = new Node(std::forward<X>(x));
        node->next = head_;
        head_ = node;
        ++size_;
    }

    struct Node
    {
        T data;
        Node* next;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* head_;
    int size_;
};


// 测试
#include <ctime>
#include <cstdlib>

int main()
{
    using namespace std;
    srand(time(nullptr));

    Stack<int> sta;
    for (int i = 0; i < 5; ++i)
    {
        int data = rand() % 100;
        sta.push(data);
        cout << "push " << data << endl;
    }
    while (!sta.empty())
    {
        cout << "pop " << sta.top() << endl;
        sta.pop();
    }
    return 0;
}
