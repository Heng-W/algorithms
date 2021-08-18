// 双向循环链表实现的线性表
#include <memory>
#include <iostream>
#include <chrono>


template <class T>
class LinkedList
{
    friend std::ostream& operator<<(std::ostream& out, const LinkedList& list)
    {
        auto cur = list.begin();
        while (cur != list.end())
        {
            out << cur->data << " ";
            cur = cur->next;
        }
        return out;
    }

public:

    struct Node
    {
        T data;
        Node* prev;
        Node* next;

        template <class X>
        Node(X&& _data): data(std::forward<X>(_data)) {}
    };

    LinkedList(): size_(0)
    {
        head_ = (Node*)::malloc(sizeof(Node));
        head_->next = head_;
        head_->prev = head_;
    }

    ~LinkedList()
    {
        clear();
        ::free(head_);
    }

    LinkedList(const LinkedList& list):
        LinkedList()
    {
        auto src = list.begin();
        while (src != list.end())
        {
            insertBack(src->data);
            src = src->next;
        }
    }

    LinkedList(LinkedList&& list):
        LinkedList()
    {
        *this = std::move(list);
    }


    LinkedList& operator=(const LinkedList& list)
    {
        LinkedList newList = list;
        return *this = std::move(newList);
    }

    LinkedList& operator=(LinkedList&& list) noexcept
    {
        if (this != &list)
        {
            using namespace std;
            swap(head_, list.head_);
            swap(size_, list.size_);
        }
        return *this;
    }

    void clear()
    {
        auto cur = begin();
        while (cur != end())
        {
            cur = cur->next;
            delete cur->prev;
        }
        head_->next = head_;
        head_->prev = head_;
    }

    //表头插入
    template <class X>
    void insertFront(X&& x) { insert(begin(), std::forward<X>(x)); }

    //表尾插入
    template <class X>
    void insertBack(X&& x) { insert(end(), std::forward<X>(x)); }

    //插入
    template <class X>
    Node* insert(Node* p, X&& x)
    {
        Node* tmp = new Node(x);
        tmp->next = p;
        tmp->prev = p->prev;
        p->prev->next = tmp;
        p->prev = tmp;
        ++size_;
        return tmp;
    }

    //删除
    Node* remove(Node* p)
    {
        p->prev->next = p->next;
        p->next->prev = p->prev;
        Node* next = p->next;
        delete p;
        --size_;
        return next;
    }

    //寻找指定结点
    const Node* find(const T& data) const
    {
        auto cur = begin();
        while (cur != end())
        {
            if (cur->data == data) return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Node* find(const T& data)
    {
        const auto* obj = this;
        return const_cast<Node*>(obj->find(data));
    }


    Node* begin() { return head_->next; }
    const Node* begin() const { return head_->next; }
    Node* end() { return head_; }
    const Node* end() const { return head_; }


    int size() const { return size_; }
    bool empty() const { return size_ == 0;}

    T& front() { return begin()->data; }
    T& back() { return end()->prev->data; }

private:
    Node* head_; //头结点
    int size_; //元素个数
};


#include <ctime>
#include <cstdlib>


int main()
{
    using namespace std;
    using namespace std::chrono;

    auto t0 = steady_clock::now();

    srand(time(nullptr));

    float a[5];
    cout << "origin: ";
    for (int i = 0; i < 5; i++)
    {
        a[i] = 100.0 * rand() / (RAND_MAX + 1.0);
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
    cout << "traversal: " << list << endl;
    cout << "size: " << list.size() << endl;

    auto list2 = std::move(list);

    cout << "traversal: " << list << endl;
    list2.find(a[4])->data = 12;
    cout << "traversal2: " << list2 << endl;


    auto t1 = steady_clock::now();
    cout << "runtime: " << duration_cast<milliseconds>(t1 - t0).count() << " ms" << endl;
    return 0;
}
