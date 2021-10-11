
#include <cstdlib>
#include <functional>

//树堆
template <class T>
class Treap
{
    struct Node;
public:
    Treap():root_(nullptr) {}
    ~Treap() { clear(); }

    //拷贝构造函数
    Treap(const Treap& rhs) { root_ = clone(rhs.root_); }

    //移动构造函数
    Treap(Treap&& rhs) noexcept: root_(rhs.root_)
    { rhs.root_ = nullptr; }

    //拷贝赋值运算符
    Treap& operator=(const Treap& rhs)
    {
        Node* newRoot = clone(rhs.root_);
        clear();
        root_ = newRoot;
        return *this;
    }

    //移动赋值运算符
    Treap& operator=(Treap&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            rhs.root_ = nullptr;
        }
        return *this;
    }

    bool insert(const T& data) { return insert(root_, data); }
    bool insert(T&& data) { return insert(root_, std::move(data)); }

    bool remove(const T& data) { return remove(root_, data); }

    Node* find(const T& data) const
    {
        Node* cur = root_;
        while (cur)
        {
            if (data < cur->data)
                cur = cur->left;
            else if (cur->data < data)
                cur = cur->right;
            else
                return cur;
        }
        return nullptr;
    }

    void clear() { destroyTree(root_); }

private:

    template <class X>
    bool insert(Node*& node, X&& x);

    bool remove(Node*& node, const T& data);

    void leftRotate(Node*& node)
    {
        Node* rchild = node->right;
        node->right = rchild->left;
        rchild->left = node;
        node = rchild;
    }

    void rightRotate(Node*& node)
    {
        Node* lchild = node->left;
        node->left = lchild->right;
        lchild->right = node;
        node = lchild;
    }

    //销毁子树
    void destroyTree(Node*& node)
    {
        if (node)
        {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
            node = nullptr;
        }
    }

    //克隆
    Node* clone(Node* node)
    {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->data, node->priority);
        copy->left = clone(node->left);
        copy->right = clone(node->right);
        return copy;
    }


    struct Node
    {
        T data;
        int priority;
        Node* left;
        Node* right;

        Node(const T& _data, int _priority) 
        : data(_data), priority(_priority) {}

        Node(T&& _data, int _priority) 
        : data(std::move(_data)), priority(_priority) {}
    };

    Node* root_;

};


template <class T>
template <class X>
bool Treap<T>::insert(Node*& node, X&& x)
{
    if (node == nullptr)
    {
        node = new Node(std::forward<X>(x), rand());
        node->left = node->right = nullptr;
    }
    else if (x < node->data)
    {
        if (!insert(node->left, std::forward<X>(x)))
            return false;
        if (node->left->priority < node->priority)
            rightRotate(node);
    }
    else if (node->data < x)
    {
        if (!insert(node->right, std::forward<X>(x)))
            return false;
        if (node->right->priority < node->priority)
            leftRotate(node);
    }
    else
    {
        return false;
    }
    return true;
}

template <class T>
bool Treap<T>::remove(Node*& node, const T& data)
{
    if (node == nullptr)
        return false;//未找到
    if (data < node->data)
        return remove(node->left, data);
    else if (node->data < data)
        return remove(node->right, data);
    //找到元素
    if (!node->left || !node->right)
    {
        Node* tmp = node;
        node = node->left ? node->left : node->right;
        delete tmp;
    }
    else
    {
        if (node->left->priority < node->right->priority)
        {
            rightRotate(node);
            remove(node->right, data);
        }
        else
        {
            leftRotate(node);
            remove(node->left, data);
        }
    }
    return true;
}


//测试
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 10; ++i)
    {
        vec.push_back(rand() % 100);
    }
    copy(vec.cbegin(), vec.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    Treap<int> treap;
    for (const auto& x:vec) treap.insert(x);

    cout << (treap.find(vec[0]) != nullptr) << endl;

    for (const auto& x:vec) treap.remove(x);

    return 0;
}
