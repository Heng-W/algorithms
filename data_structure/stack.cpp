// 链表实现的堆栈
#include <assert.h>
#include <memory>
#include <iostream>


template <class T>
class Stack
{
    friend void swap(Stack& stack1, Stack& stack2)
    {
        stack1.swap(stack2);
    }
public:
    struct Node
    {
        T data;
        Node* next;

        template <class X>
        Node(X&& _data): data(std::forward<X>(_data)) {}
    };

    Stack(): head_(nullptr), size_(0) {}

    ~Stack() { clear(); }

    Stack(const Stack& stack)
        : head_(nullptr),
          size_(0)
    {
        auto src = stack.head_;
        while (src)
        {
            push(src->data);
            src = src->next;
        }
    }

    Stack(Stack&& stack) noexcept
        : head_(stack.head_),
          size_(stack.size)
    {
        stack.head_ = nullptr;
        stack.size_ = 0;
    }

    Stack& operator=(const Stack& stack)
    {
        Stack tmp = stack;
        swap(tmp);
        return *this;
    }

    Stack& operator=(Stack&& stack) noexcept
    {
        if (this != &stack)
        {
            clear();
            head_ = stack.head_;
            size_ = stack.size_;

            stack.head_ = nullptr;
            stack.size_ = 0;
        }
        return *this;
    }

    void swap(Stack& stack)
    {
        using std::swap;
        swap(head_, stack.head_);
        swap(size_, stack.size_);
    }

    //加入元素
    template <class X>
    void push(X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->next = head_;
        head_ = newNode;
        ++size_;
    }

    //弹出
    T pop()
    {
        assert(!empty());
        Node* tmp = head_;
        T data = std::move(tmp->data);
        head_ = head_->next;
        delete tmp;
        --size_;
        return data;
    }

    void clear()
    {
        auto cur = head_;
        while (cur)
        {
            auto tmp = cur;
            cur = cur->next;
            delete tmp;
        }
        size_ = 0;
    }


    bool empty() const { return size_ == 0; }

    int size() const { return size_; }

private:
    Node* head_;
    int size_;
};


#include <ctime>
#include <cstdlib>

int main()
{
    using namespace std;
    srand(time(nullptr));

    Stack<int> s;
    for (int i = 0; i < 5; ++i)
    {
        int data = 100.0 * rand() / RAND_MAX;
        s.push(data);
        cout << "push " << data << endl;
    }
    while (!s.empty())
    {
        cout << "pop " << s.pop() << endl;
    }
    return 0;
}
