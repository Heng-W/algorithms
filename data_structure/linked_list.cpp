
#include <memory>

// 双向循环链表实现的线性表
template <class T>
class LinkedList
{
    template <class NodePtr> struct IteratorT;
    struct Node;
public:
    using Iterator = IteratorT<Node*>;
    using ConstIterator = IteratorT<const Node*>;

    LinkedList(): size_(0)
    {
        head_ = static_cast<Node*>(::malloc(sizeof(Node)));
        head_->next = head_;
        head_->prev = head_;
    }

    ~LinkedList() { clear(); ::free(head_); }

    // 拷贝构造函数
    LinkedList(const LinkedList& rhs)
        : LinkedList()
    {
        auto src = rhs.begin();
        while (src != rhs.end())
        {
            insertBack(src->data);
            src = src->next;
        }
    }

    // 移动构造函数
    LinkedList(LinkedList&& rhs): LinkedList()
    { swap(rhs); }

    // 拷贝赋值运算符
    LinkedList& operator=(const LinkedList& rhs)
    {
        LinkedList().swap(*this);
        return *this;
    }

    // 移动赋值运算符
    LinkedList& operator=(LinkedList&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            rhs.swap(*this);
        }
        return *this;
    }

    void swap(LinkedList& rhs)
    {
        using std::swap;
        swap(head_, rhs.head_);
        swap(size_, rhs.size_);
    }

    // 插入
    void insertFront(const T& x) { insert(begin(), x); }
    void insertFront(T&& x) { insert(begin(), std::move(x)); }

    void insertBack(const T& x) { insert(end(), x); }
    void insertBack(T&& x) { insert(end(), std::move(x)); }

    Iterator insert(Iterator pos, const T& x) { return _insert(pos, x); }
    Iterator insert(Iterator pos, T&& x) { return _insert(pos, std::move(x)); }

    // 查找
    ConstIterator find(const T& data) const
    { return _find(data); }

    Iterator find(const T& data)
    { return const_cast<Node*>(_find(data)); }

    // 删除
    Iterator remove(Iterator pos)
    {
        Node* p = pos.node;
        p->prev->next = p->next;
        p->next->prev = p->prev;
        Node* next = p->next;
        delete p;
        --size_;
        return next;
    }

    void clear()
    {
        Node* cur = head_->next;
        while (cur != head_)
        {
            cur = cur->next;
            delete cur->prev;
        }
        head_->next = head_;
        head_->prev = head_;
        size_ = 0;
    }

    Iterator begin() { return head_->next; }
    ConstIterator begin() const { return head_->next; }
    Iterator end() { return head_; }
    ConstIterator end() const { return head_; }

    const T& front() const { return head_->next->data; }
    T& front() { return head_->next->data; }
    const T& back() const { return head_->prev->data; }
    T& back() { return head_->prev->data; }

    int size() const { return size_; }
    bool empty() const { return size_ == 0;}

private:

    const Node* _find(const T& data) const
    {
        const Node* cur = head_->next;
        while (cur != head_)
        {
            if (cur->data == data) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    template <class X>
    Iterator _insert(Iterator pos, X&& x)
    {
        Node* p = pos.node;
        Node* node = new Node(x);
        node->next = p;
        node->prev = p->prev;
        p->prev->next = node;
        p->prev = node;
        ++size_;
        return node;
    }

    // 迭代器
    template <class NodePtr>
    struct IteratorT
    {
        NodePtr node;

        using Self = IteratorT;
        using ObjectRef = decltype((node->data));
        using ObjectPtr = decltype(&node->data);

        IteratorT() {}
        IteratorT(NodePtr _node): node(_node) {}

        bool operator==(const Self& it) const { return node == it.node; }
        bool operator!=(const Self& it) const { return node != it.node; }

        ObjectRef operator*() const { return node->data; }
        ObjectPtr operator->() const { return &*this; }

        Self& operator++()
        {
            node = node->next;
            return *this;
        }
        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self& operator--()
        {
            node = node->prev;
            return *this;
        }
        Self operator--(int)
        {
            Self tmp = *this;
            --*this;
            return tmp;
        }
    };

    struct Node
    {
        T data;
        Node* prev;
        Node* next;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* head_; // 头结点
    int size_; // 元素个数
};


// 测试
#include <cstdlib>
#include <ctime>
#include <iostream>

int main()
{
    using namespace std;
    srand(time(nullptr));

    float a[5];
    for (int i = 0; i < 5; i++)
    {
        a[i] = rand() % 100;
        cout << a[i] << "  ";
    }
    cout << endl;

    LinkedList<float> list;
    list.insertFront(a[0]);
    list.insert(list.find(a[0]), a[1]);
    list.insert(list.find(a[0]), a[2]);
    list.insertBack(a[3]);
    list.remove(list.find(a[2]));
    list.insertFront(a[4]);
    for (const auto& x : list) cout << x << " ";
    cout << endl;
    cout << "size: " << list.size() << endl;

    auto list2 = std::move(list);

    for (const auto& x : list) cout << x << " ";
    cout << endl;

    *list2.find(a[4]) = 12;
    for (const auto& x : list2) cout << x << " ";
    cout << endl;

    return 0;
}
