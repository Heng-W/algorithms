//数组实现的环形队列

#include <assert.h>
#include <memory>


template <class T>
class CircularQueue
{
public:
    CircularQueue(int cap = 32):
        front_(0), rear_(0)
    {
        cap_ = roundupPowerOfTwo(cap);
        data_ = alloc_.allocate(cap_);
    }

    ~CircularQueue() { free(); }


    void pop()
    {
        assert(!empty());
        alloc_.destroy(&data_[front_++]);
        front_ &= cap_ - 1;
    }

    void push(const T& x) {_push(x);}
    void push(T&& x) {_push(std::move(x));}

    void clear()
    {
        while (!empty())
        {
            pop();
        }
    }

    void free()
    {
        clear();
        if (data_)
        {
            alloc_.deallocate(data_, cap_);
        }
    }


    T& top() const { return data_[front_]; }

    int count() const { return (rear_ - front_ + cap_) & (cap_ - 1); }
    int cap() const { return cap_; }

    bool empty() const { return front_ == rear_; }
    bool full() const { return front_ == ((rear_ + 1) & (cap_ - 1)); }

private:

    template <class X>
    void _push(X&& x)
    {
        if (full())
        {
            resize();
        }
        alloc_.construct(&data_[rear_++], std::forward<X>(x));
        rear_ &= cap_ - 1;
    }

    void resize()
    {
        unsigned int newCap = cap_ * 2;
        T* newData = alloc_.allocate(newCap);

        unsigned int dst = 0;
        unsigned int src = front_;
        while (src != rear_)
        {
            alloc_.construct(&newData[dst++], std::move(data_[src++]));
            src &= cap_ - 1;
        }
        free();
        data_ = newData;
        cap_ = newCap;
        front_ = 0;
        rear_ = dst;
    }


    unsigned int roundupPowerOfTwo(unsigned int i)
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
    unsigned int cap_;//容量
    unsigned int front_;
    unsigned int rear_;
};


#include <iostream>

int main()
{
    using namespace std;

    CircularQueue<int> q(4);
    q.push(10);
    q.push(20);
    q.pop();
    q.push(30);
    q.push(40);
    q.push(50);
    q.push(60);
    cout << q.count() <<endl;
    cout << q.cap() <<endl;
    while (!q.empty())
    {
        cout << q.top() << " deQueue" << endl;
        q.pop();
    }
    return 0;
}
