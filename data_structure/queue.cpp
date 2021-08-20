// 链表实现的队列
#include <assert.h>
#include <memory>
#include <iostream>


template <class T>
class Queue
{
    friend void swap(Queue& queue1, Queue& queue2)
    {
        queue1.swap(queue2);
    }
public:
    struct Node
    {
        T data;
        Node* next;

        template <class X>
        Node(X&& _data): data(std::forward<X>(_data)) {}
    };

    Queue(): head_(nullptr), tail_(nullptr), size_(0) {}

    ~Queue() { clear(); }

    Queue(const Queue& queue)
        : head_(nullptr), tail_(nullptr), size_(0)
    {
        auto src = queue.head_;
        while (src)
        {
            enQueue(src->data);
            src = src->next;
        }
    }

    Queue(Queue&& queue) noexcept
        : head_(queue.head_),
          tail_(queue.tail_),
          size_(queue.size)
    {
        queue.head_ = queue.tail_ = nullptr;
        queue.size_ = 0;
    }

    Queue& operator=(const Queue& queue)
    {
        Queue tmp = queue;
        swap(tmp);
        return *this;
    }

    Queue& operator=(Queue&& queue) noexcept
    {
        if (this != &queue)
        {
            clear();
            head_ = queue.head_;
            tail_ = queue.tail_;
            size_ = queue.size_;

            queue.head_ = queue.tail_ = nullptr;
            queue.size_ = 0;
        }
        return *this;
    }

    void swap(Queue& queue)
    {
        using std::swap;
        swap(head_, queue.head_);
        swap(tail_, queue.tail_);
        swap(size_, queue.size_);
    }

    //加入队列
    template <class X>
    void enQueue(X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->next = nullptr;
        if (empty())
        {
            head_ = tail_ = newNode;
        }
        else
        {
            tail_->next = newNode;
            tail_ = newNode;
        }
        ++size_;
    }

    //弹出队列
    T deQueue()
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
    Node* tail_;

    int size_;
};


#include <ctime>
#include <cstdlib>

int main()
{
    using namespace std;
    srand(time(nullptr));

    Queue<int> q;
    for (int i = 0; i < 5; ++i)
    {
        int data = 100.0 * rand() / RAND_MAX;
        q.enQueue(data);
        cout << data << " enQueue" << endl;
    }
    while (!q.empty())
    {
        cout << q.deQueue() << " deQueue" << endl;
    }
    return 0;
}
