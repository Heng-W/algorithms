// 二叉搜索树
#include <vector>


template <class T>
class BinarySearchTree
{
    struct Node;
public:
    BinarySearchTree(): root_(nullptr) {}

    BinarySearchTree(const T* begin, const T* end)
        : root_(nullptr) { insertRange(begin, end); }

    ~BinarySearchTree() { clear(); }

    //插入节点（递归）
    bool insert(const T& x) { return _insert(root_, x); }
    bool insert(T&& x) { return _insert(root_, std::move(x)); }

    //插入节点（非递归）
    bool insertByIter(const T& x) { return _insertByIter(x); }
    bool insertByIter(T&& x)  { return _insertByIter(std::move(x)); }

    //批量插入（拷贝）
    void insertRange(const T* begin, const T* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(*p);
    }

    //批量插入（移动）
    void emplaceRange(T* begin, T* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(std::move(*p));
    }

    //查找节点（递归）
    Node* find(const T& data) { return _find(root_, data); }

    //查找节点（非递归）
    Node* findByIter(const T& data);

    //删除（递归）
    bool remove(const T& data) { return remove(root_, data); }

    //删除（非递归）
    bool removeByIter(const T& data);

    //中序遍历
    std::vector<T> inorder() const
    {
        std::vector<T> res;
        inorder(root_, res);
        return res;
    }

    int depth() const { return depth(root_); }

    void clear() { destroy(root_); }

private:
    template <class X>
    bool _insert(Node*& node, X&& x);

    template <class X>
    bool _insertByIter(X&& x);

    Node* _find(Node* node, const T& data);

    bool remove(Node*& node, const T& data);

    void removeNode(Node*& node);


    int depth(Node* node) const
    {
        return node ? std::max(depth(node->left), depth(node->right)) + 1 : 0;
    }

    //中序遍历
    void inorder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            inorder(node->left, res);
            res.push_back(node->data);
            inorder(node->right, res);
        }
    }

    //销毁
    void destroy(Node*& node)
    {
        if (node)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };
    Node* root_;
};

template <class T>
template <class X>
bool BinarySearchTree<T>::_insert(Node*& node, X&& x)
{
    if (node == nullptr)
    {
        node = new Node(std::forward<X>(x));
        node->left = node->right = nullptr;
        return true;
    }
    if (x < node->data)
        return _insert(node->left, std::forward<X>(x));
    else if (node->data < x)
        return _insert(node->right, std::forward<X>(x));
    else
        return false;
}

template <class T>
template <class X>
bool BinarySearchTree<T>::_insertByIter(X&& x)
{
    Node* parent = nullptr;
    Node* cur = root_;
    while (cur)
    {
        parent = cur;
        if (x < cur->data)
            cur = cur->left;
        else if (cur->data < x)
            cur = cur->right;
        else
            return false;
    }
    Node* node = new Node(std::forward<X>(x));
    node->left = node->right = nullptr;

    if (root_ == nullptr)
        root_ = node;
    else if (x < parent->data)
        parent->left = node;
    else
        parent->right = node;
    return true;
}

template <class T>
auto BinarySearchTree<T>::_find(Node* node, const T& data) ->Node*
{
    if (node == nullptr)
        return nullptr;
    else if (data < node->data)
        return _find(node->left, data);
    else if (node->data < data)
        return _find(node->right, data);
    else
        return node;
}

template <class T>
auto BinarySearchTree<T>::findByIter(const T& data) ->Node*
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

template <class T>
bool BinarySearchTree<T>::remove(Node*& node, const T& data)
{
    if (node == nullptr)
        return false;//未找到
    else if (data < node->data)
        return remove(node->left, data);
    else if (node->data < data)
        return remove(node->right, data);
    else
        removeNode(node);//删除节点
    return true;
}

template <class T>
bool BinarySearchTree<T>::removeByIter(const T& data)
{
    Node* cur = root_;
    Node* parent = nullptr;
    while (cur)
    {
        if (data == cur->data)
            break;
        parent = cur;
        if (data < cur->data)
            cur = cur->left;
        else
            cur = cur->right;
    }
    if (cur == nullptr) return false;
    if (cur == root_)
        removeNode(root_);
    else if (parent->left == cur)
        removeNode(parent->left);
    else
        removeNode(parent->right);
    return true;
}

template <class T>
void BinarySearchTree<T>::removeNode(Node*& node)
{
    if (node->left && node->right)
    {
        Node* parent = node;
        Node* sub = node->right;
        while (sub->left)
        {
            parent = sub;
            sub = sub->left;
        }
        node->data = std::move(sub->data);//被删节点后继
        if (parent != node)
            parent->left = sub->right;
        else
            parent->right = sub->right;
        delete sub;
    }
    else
    {
        Node* tmp = node;
        node = (node->left != nullptr) ? node->left : node->right;
        delete tmp;
    }
}


#include <cstdlib>
#include <ctime>
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

    BinarySearchTree<int> tree(&*vec.cbegin(), &*vec.cend());
    auto res = tree.inorder();
    copy(res.cbegin(), res.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    for (int i = 0; i < 5; ++i)
    {
        tree.remove(vec[i]);
    }
    res = tree.inorder();
    copy(res.cbegin(), res.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    return 0;
}
