
#include <assert.h>
#include <memory>

//数组实现的环形队列
template <class T>
class CircularQueue
{
public:
    CircularQueue(int capacity = 32)
    :front_(0), rear_(0)
    {
        capacity_ = roundupPowerOfTwo(capacity);
        data_ = alloc_.allocate(capacity_);
    }

    ~CircularQueue() { free(); }

    CircularQueue(const CircularQueue& rhs)
    :front_(0), rear_(0), capacity_(rhs.capacity_)
    {
        data_ = alloc_.allocate(capacity_);
        unsigned int src = rhs.front_;
        while (src != rhs.rear_)
        {
            alloc_.construct(&data_[rear_++], std::move(data_[src++]));
            src &= capacity_ - 1;
        }
    }

    CircularQueue(CircularQueue&& rhs)
    :CircularQueue(1) { swap(rhs); }


    CircularQueue& operator=(const CircularQueue& rhs)
    {
        CircularQueue copy = rhs;
        swap(copy);
        return *this;
    }

    CircularQueue& operator=(CircularQueue&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    void swap(CircularQueue& rhs) 
    {
        using std::swap;
        swap(data_,rhs.data_);
        swap(capacity_, rhs.capacity_);
        swap(front_, rhs.front_);
        swap(rear_, rhs.rear_);
    }

    void push(const T& x) { _push(x); }
    void push(T&& x) { _push(std::move(x)); }

    void pop()
    {
        assert(!empty());
        alloc_.destroy(&data_[front_++]);
        front_ &= capacity_ - 1;
    }

    void clear()
    {
        while (!empty())
        {
            pop();
        }
    }

    const T& front() const { return data_[front_]; }

    int count() const { return (rear_ - front_ + capacity_) & (capacity_ - 1); }
    int capacity() const { return capacity_; }

    bool empty() const { return front_ == rear_; }
    bool full() const { return front_ == ((rear_ + 1) & (capacity_ - 1)); }

private:

    template <class X>
    void _push(X&& x)
    {
        if (full())
        {
            resize();
        }
        alloc_.construct(&data_[rear_++], std::forward<X>(x));
        rear_ &= capacity_ - 1;
    }

    //扩容
    void resize()
    {
        unsigned int newCap = capacity_ * 2;
        T* newData = alloc_.allocate(newCap);

        unsigned int dst = 0;
        unsigned int src = front_;
        while (src != rear_)
        {
            alloc_.construct(&newData[dst++], std::move(data_[src++]));
            src &= capacity_ - 1;
        }
        free();
        data_ = newData;
        capacity_ = newCap;
        front_ = 0;
        rear_ = dst;
    }

    void free()
    {
        clear();
        if (data_) alloc_.deallocate(data_, capacity_);
    }

    //取整为2的幂次方
    static unsigned int roundupPowerOfTwo(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i + 1;
    }

    static std::allocator<T> alloc_;
    T* data_;
    unsigned int capacity_;//容量
    unsigned int front_;
    unsigned int rear_;
};


//测试
#include <iostream>

int main()
{
    using namespace std;

    CircularQueue<int> que(4);
    que.push(10);
    que.push(20);
    que.push(30);
    que.pop();
    que.pop();
    que.push(40);
    que.push(50);
    que.push(60);
    cout << que.count() <<endl;
    cout << que.capacity() <<endl;
    while (!que.empty())
    {
        cout << que.front() << " deQueue" << endl;
        que.pop();
    }
    return 0;
}
