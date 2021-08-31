// 动态数组实现的线性表
#include <memory>
#include <iostream>
#include <chrono>


template <class T>
class ArrayList
{
    friend void swap(ArrayList& list1, ArrayList& list2)
    {
        list1.swap(list2);
    }

    friend std::ostream& operator<<(std::ostream& out, const ArrayList& list)
    {
        for (int i = 0; i < list.size_; ++i)
        {
            out << list.data_[i] << " ";
        }
        return out;
    }
public:
    ArrayList(): data_(nullptr), size_(0), cap_(0) {}

    explicit ArrayList(int size): size_(size), cap_(size)
    {
        data_ = alloc_.allocate(cap_);
        std::uninitialized_fill_n(data_, size, T());
    }

    ArrayList(int size, const T& value): size_(size), cap_(size)
    {
        data_ = alloc_.allocate(cap_);
        std::uninitialized_fill_n(data_, size, value);
    }

    ~ArrayList() { free(); }

    ArrayList(const ArrayList& list)
    {
        data_ = alloc_.allocate(list.size_);
        std::uninitialized_copy_n(list.data_, list.size_, data_);
        size_ = cap_ = list.size_;
    }

    ArrayList(ArrayList&& list) noexcept
        : data_(list.data_), size_(list.size_), cap_(list.cap_)
    {
        list.data_ = nullptr;
        size_ = cap_ = 0;
    }

    ArrayList& operator=(const ArrayList& list)
    {
        auto data = alloc_.allocate(list.size_);
        std::uninitialized_copy_n(list.data_, list.size_, data);
        free();
        data_ = data;
        size_ = cap_ = list.size_;
        return *this;
    }

    ArrayList& operator=(ArrayList&& list) noexcept
    {
        if (this != &list)
        {
            free();
            //接管资源
            data_ = list.data_;
            size_ = list.size_;
            cap_ = list.cap_;
            //使原资源可安全析构
            list.data_ = nullptr;
            size_ = cap_ = 0;
        }
        return *this;
    }

    void swap(ArrayList& list)
    {
        using std::swap;
        swap(data_, list.data_);
        swap(size_, list.size_);
        swap(cap_, list.cap_);
    }

    void add(const T& x) { _add(x); }
    void add(T&& x) { _add(std::move(x)); }

    void insert(int idx, const T& x) { _insert(idx, x); }
    void insert(int idx, T&& x) { _insert(idx, std::move(x)); }


    int find(const T& x)
    {
        for (int i = 0; i < size_; ++i)
            if (data_[i] == x)
                return i;
        return -1;
    }

    void remove(int idx)
    {
        std::copy(data_ + idx + 1, data_ + size_, data_ + idx);
        --size_;
        alloc_.destroy(data_ + size_);
    }

    void remove(int pos, int cnt)
    {
        std::copy(data_ + pos + cnt, data_ + size_, data_ + pos);
        for (int i = size_ - cnt; i < size_; ++i)
        {
            alloc_.destroy(data_ + i);
        }
        size_ -= cnt;
    }

    //反转
    void reverse()
    {
        using std::swap;
        for (int i = 0; i < size_ / 2; ++i)
        {
            swap(data_[i], data_[size_ - i - 1]);
        }
    }

    //弹出末尾元素
    void pop()
    {
        --size_;
        alloc_.destroy(data_ + size_);
    }

    void clear()
    {
        for (int i = size_ - 1; i >= 0; --i)
        {
            alloc_.destroy(data_ + i);
        }
        size_ = 0;
    }

    T& front() { return data_[0]; }
    T& back() { return data_[size_ - 1]; }

    T* begin() { return data_; }
    T* end() { return data_ + size_; }

    T& operator[](int i) { return data_[i]; }
    const T& operator[](int i) const { return data_[i]; }

    bool empty() const { return size_ == 0; }
    bool full() const { return size_ == cap_; }

    const T* data() const { return data_; }
    int cap() const { return cap_; }
    int size() const { return size_; }

private:

    template <class X>
    void _add(X&& x)
    {
        if (!full())
        {
            alloc_.construct(data_ + size_, std::forward<X>(x));
            ++size_;
        }
        else
        {
            _insert(size_, std::forward<X>(x));
        }
    }

    template <class X>
    void _insert(int idx, X&& x)
    {
        if (!full())
        {
            alloc_.construct(data_ + size_, std::move(data_[size_ - 1]));//末尾构造一个元素
            ++size_;
            //元素后移
            for (int i = size_ - 2; i > idx; --i)
                alloc_.construct(data_ + i, std::move(data_[i - 1]));
            data_[idx] = std::forward<X>(x);
        }
        else
        {
            //扩容，拷贝到新地址
            int newCap = cap_ != 0 ? 2 * cap_ : 1;
            auto newData = alloc_.allocate(newCap);

            for (int i = 0; i < idx; ++i)
                alloc_.construct(newData + i, std::move(data_[i]));
            alloc_.construct(newData + idx, std::forward<X>(x));
            for (int i = idx; i < size_; ++i)
                alloc_.construct(newData + i + 1, std::move(data_[i]));

            free();
            //调整为新数据
            data_ = newData;
            ++size_;
            cap_ = newCap;
        }
    }

    void free()
    {
        if (data_)
        {
            for (int i = size_ - 1; i >= 0; --i)
                alloc_.destroy(data_ + i);
            alloc_.deallocate(data_, cap_);
        }
    }

    static std::allocator<T> alloc_;
    T* data_;
    int size_;//实际大小
    int cap_;//容量
};



int main()
{
    using namespace std;
    using namespace std::chrono;
    auto t0 = steady_clock::now();

    ArrayList<int> list;
    list.add(88);
    list.add(23);
    list.insert(0, 55);
    int val = 90;
    list.insert(0, val);
    val = 77;
    list.add(val);
    list.reverse();
    cout << "reverse: " << list << endl;

    auto list2 = list;
    cout << "list2: " << list2 << endl;

    ArrayList<std::unique_ptr<int>> list3;
    list3.add(unique_ptr<int>(new int(3)));

    auto list4 = std::move(list3);

    auto t1 = steady_clock::now();
    cout << "runtime: " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << endl;

    return 0;
}
