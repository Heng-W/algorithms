
#include <memory>

//动态数组实现的线性表
template <class T>
class ArrayList
{
public:
    //默认构造函数
    ArrayList(): data_(nullptr), size_(0), capacity_(0) {}

    //构造函数，创建n个值初始化的元素
    explicit ArrayList(int n): ArrayList(n, T()) {}

    //构造函数，创建n个值为value的元素
    ArrayList(int n, const T& value)
        : size_(n), capacity_(n)
    {
        data_ = alloc_.allocate(capacity_);
        std::uninitialized_fill_n(data_, n, value);
    }

    //析构函数
    ~ArrayList() { free(); }

    //拷贝构造函数
    ArrayList(const ArrayList& rhs)
        : size_(rhs.size_), capacity_(rhs.size_)
    {
        data_ = alloc_.allocate(capacity_);
        std::uninitialized_copy_n(rhs.data_, rhs.size_, data_);
    }

    //移动构造函数
    ArrayList(ArrayList&& rhs) noexcept
        : data_(rhs.data_), size_(rhs.size_), capacity_(rhs.capacity_)
    {
        rhs.data_ = nullptr;
        rhs.size_ = rhs.capacity_ = 0;
    }

    //拷贝赋值运算符
    ArrayList& operator=(const ArrayList& rhs)
    {
        T* data = alloc_.allocate(rhs.size_);
        std::uninitialized_copy_n(rhs.data_, rhs.size_, data);
        free();
        data_ = data;
        size_ = capacity_ = rhs.size_;
        return *this;
    }

    //移动赋值运算符
    ArrayList& operator=(ArrayList&& rhs) noexcept
    {
        if (this != &rhs)
        {
            free();
            //接管资源
            data_ = rhs.data_;
            size_ = rhs.size_;
            capacity_ = rhs.capacity_;
            //清除rhs
            rhs.data_ = nullptr;
            rhs.size_ = rhs.capacity_ = 0;
        }
        return *this;
    }

    //末尾添加元素
    void add(const T& x) { _add(x); }
    void add(T&& x) { _add(std::move(x)); }

    //在pos位置插入元素
    void insert(int pos, const T& x) { _insert(pos, x); }
    void insert(int pos, T&& x) { _insert(pos, std::move(x)); }

    //查找
    int find(const T& x) const
    {
        for (int i = 0; i < size_; ++i)
        {
            if (data_[i] == x)
                return i;
        }
        return -1;
    }

    //删除pos位置的元素
    void remove(int pos)
    {
        std::copy(data_ + pos + 1, data_ + size_, data_ + pos);
        --size_;
        alloc_.destroy(data_ + size_);
    }

    //删除pos位置的连续count个元素
    void remove(int pos, int count)
    {
        std::copy(data_ + pos + count, data_ + size_, data_ + pos);
        for (int i = size_ - count; i < size_; ++i)
        {
            alloc_.destroy(data_ + i);
        }
        size_ -= count;
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

    //删除末尾元素
    void removeBack()
    {
        --size_;
        alloc_.destroy(data_ + size_);
    }

    //清除元素
    void clear()
    {
        for (int i = size_ - 1; i >= 0; --i)
        {
            alloc_.destroy(data_ + i);
        }
        size_ = 0;
    }

    const T& operator[](int i) const { return data_[i]; }
    T& operator[](int i) { return data_[i]; }

    //首尾迭代器
    const T* begin() const { return data_; }
    T* begin() { return data_; }

    const T* end() const { return data_ + size_; }
    T* end() { return data_ + size_; }

    //首尾元素
    const T& front() const { return data_[0]; }
    T& front() { return data_[0]; }

    const T& back() const { return data_[size_ - 1]; }
    T& back() { return data_[size_ - 1]; }

    const T* data() const { return data_; }
    bool empty() const { return size_ == 0; }
    int capacity() const { return capacity_; }
    int size() const { return size_; }

private:

    template <class X>
    void _add(X&& x)
    {
        if (size_ < capacity_)
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
    void _insert(int pos, X&& x)
    {
        if (size_ < capacity_)
        {
            alloc_.construct(data_ + size_, std::move(data_[size_ - 1]));//末尾构造一个元素
            ++size_;
            //元素后移
            for (int i = size_ - 2; i > pos; --i)
                alloc_.construct(data_ + i, std::move(data_[i - 1]));
            data_[pos] = std::forward<X>(x);
        }
        else
        {
            //扩容，拷贝到新地址
            int newCap = capacity_ != 0 ? 2 * capacity_ : 1;
            auto newData = alloc_.allocate(newCap);

            for (int i = 0; i < pos; ++i)
                alloc_.construct(newData + i, std::move(data_[i]));
            alloc_.construct(newData + pos, std::forward<X>(x));
            for (int i = pos; i < size_; ++i)
                alloc_.construct(newData + i + 1, std::move(data_[i]));

            free();
            //调整为新数据
            data_ = newData;
            ++size_;
            capacity_ = newCap;
        }
    }

    //清空元素并释放内存
    void free()
    {
        clear();
        if (data_) alloc_.deallocate(data_, capacity_);
    }

    static std::allocator<T> alloc_;
    T* data_; //指向数据
    int size_; //实际大小
    int capacity_; //容量
};


//测试
#include <iostream>

int main()
{
    using namespace std;

    ArrayList<int> list;
    list.add(10);
    list.add(20);
    list.insert(0, 5);
    list.insert(0, 1);
    list.add(30);
    list.reverse();

    for (const auto& x : list) cout << x << " ";

    return 0;
}
