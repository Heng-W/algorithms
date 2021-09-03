// 二叉搜索树
#include <vector>
#include <queue>


template <class T>
class BinarySearchTree
{
    class Node;
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
    bool deleteData(const T& data) { return deleteData(root_, data); }

    //删除（非递归）
    bool deleteDataByIter(const T& data);

    //中序遍历
    std::vector<T> inorder() const
    {
        std::vector<T> res;
        inorder(root_, res);
        return res;
    }

    int depth() const { return depth(root_); }

    void clear();

private:
    template <class X>
    bool _insert(Node*& node, X&& x);

    template <class X>
    bool _insertByIter(X&& x);

    Node* _find(Node* node, const T& data);

    bool deleteData(Node*& node, const T& data);

    void deleteNode(Node*& node);


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
    Node* newNode = new Node(std::forward<X>(x));
    newNode->left = newNode->right = nullptr;

    if (root_ == nullptr)
    {
        root_ = newNode;
        return true;
    }
    Node* cur = root_;
    Node* parent = nullptr;
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
    if (x < parent->data)
        parent->left = newNode;
    else
        parent->right = newNode;
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
bool BinarySearchTree<T>::deleteData(Node*& node, const T& data)
{
    if (node == nullptr)
        return false;//未找到
    else if (data < node->data)
        return deleteData(node->left, data);
    else if (node->data < data)
        return deleteData(node->right, data);
    else
        deleteNode(node);//删除节点
    return true;
}

template <class T>
bool BinarySearchTree<T>::deleteDataByIter(const T& data)
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
        deleteNode(root_);
    else if (parent->left == cur)
        deleteNode(parent->left);
    else
        deleteNode(parent->right);
    return true;
}

template <class T>
void BinarySearchTree<T>::deleteNode(Node*& node)
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

template <class T>
void BinarySearchTree<T>::clear()
{
    if (root_ == nullptr) return;
    std::queue<Node*> nodes;
    nodes.push(root_);
    while (!nodes.empty())
    {
        Node* cur = nodes.front();
        nodes.pop();
        if (cur->left) nodes.push(cur->left);
        if (cur->right) nodes.push(cur->right);
        delete cur;
    }
    root_ = nullptr;
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
        tree.deleteData(vec[i]);
    }
    res = tree.inorder();
    copy(res.cbegin(), res.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    return 0;
}
