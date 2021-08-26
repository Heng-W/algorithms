// 二叉搜索树
#include <iostream>


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
    template <class X>
    bool insert(X&& x) { return insert(root_, std::forward<X>(x)); }

    //插入节点（非递归）
    template <class X>
    bool insertByIter(X&& x);

    //批量插入（拷贝）
    void insertRange(const T* begin, const T* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(*p);
    }

    //批量插入（移动）
    void moveInsertRange(T* begin, T* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(std::move(*p));
    }

    //查找节点（递归）
    Node* find(Node* node, const T& data);

    //查找节点（非递归）
    Node* findByIter(Node* node, const T& data);

    //删除（递归）
    bool deleteData(const T& data) { return deleteData(root_, data); }

    //删除（非递归）
    bool deleteDataByIter(const T& data);

    //前序，中序，后序遍历
    void preorder() const { preorder(root_); }
    void inorder() const { inorder(root_); }
    void postorder() const { postorder(root_); }

    int depth() const { return depth(root_); }

    void clear() { deleteTree(root_); }

private:
    template <class X>
    bool insert(Node*& node, X&& x);

    bool deleteData(Node*& node, const T& data);

    void deleteNode(Node*& node);


    int depth(Node* node) const
    {
        return node ? std::max(depth(node->left), depth(node->right)) + 1 : 0;
    }

    //前序遍历
    void preorder(Node* node) const
    {
        if (node)
        {
            std::cout << node->data << " ";
            preorder(node->left);
            preorder(node->right);
        }
    }

    //中序遍历
    void inorder(Node* node) const
    {
        if (node)
        {
            inorder(node->left);
            std::cout << node->data << " ";
            inorder(node->right);
        }
    }

    //后序遍历
    void postorder(Node* node) const
    {
        if (node)
        {
            postorder(node->left);
            postorder(node->right);
            std::cout << node->data << " ";
        }
    }
    
    //删除子树
    void deleteTree(Node*& node)
    {
        _deleteTree(node);
        node = nullptr;
    }

    void _deleteTree(Node* node)
    {
        if (node)
        {
            _deleteTree(node->left);
            _deleteTree(node->right);
            delete node;
        }
    }

    struct Node
    {
        T data;
        Node* left;
        Node* right;

        template <class X>
        Node(X&& _data): data(std::forward<X>(_data)) {}
    };
    Node* root_;
};

template <class T>
template <class X>
bool BinarySearchTree<T>::insert(Node*& node, X&& x)
{
    if (node == nullptr)
    {
        node = new Node(std::forward<X>(x));
        node->left = node->right = nullptr;
        return true;
    }
    if (x < node->data)
        return insert(node->left, std::forward<X>(x));
    else if (node->data < x)
        return insert(node->right, std::forward<X>(x));
    else
        return false;
}

template <class T>
template <class X>
bool BinarySearchTree<T>::insertByIter(X&& x)
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
typename BinarySearchTree<T>::Node* 
BinarySearchTree<T>::find(Node* node, const T& data)
{
    if (node == nullptr)
        return nullptr;
    if (data < node->data)
        return find(node->left, data);
    else if (node->data < data)
        return find(node->right, data);
    else 
        return node;
}

template <class T>
typename BinarySearchTree<T>::Node* 
BinarySearchTree<T>::findByIter(Node* node, const T& data)
{
    while (node)
    {
        if (data < node->data)
            node = node->left;
        else if (node->data < data)
            node = node->right;
        else
            return node;
    }
    return nullptr;
}

template <class T>
bool BinarySearchTree<T>::deleteData(Node*& node, const T& data)
{
    if (node == nullptr)
        return false;//未找到
    if (data == node->data)
    {
        deleteNode(node);//删除节点
        return true;
    }
    if (data < node->data)
        return deleteData(node->left, data);
    else
        return deleteData(node->right, data);
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
    if (node->left == nullptr) //左节点为空
    {
        Node* tmp = node;
        node = node->right;
        delete tmp;
    }
    else if (node->right == nullptr) //右节点为空
    {
        Node* tmp = node;
        node = node->left;
        delete tmp;
    }
    else //左右节点都不为空
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
}

#include <stdlib.h>
#include <time.h>

int main()
{
    using namespace std;
    constexpr int N = 10;
    int num[N];

    srand(time(nullptr));
    for (int i = 0; i < N; ++i)
    {
        num[i] = rand() % 128;
    }

    BinarySearchTree<int> tree(num, num + N);
    tree.inorder();
    cout << endl;

    tree.deleteData(num[0]);
    tree.deleteData(num[1]);
    tree.inorder();
    cout << endl;

    return 0;
}
