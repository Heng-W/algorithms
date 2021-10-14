
#include <iostream>

// 伸展树
template <class T>
class SplayTree
{
    struct Node;
public:
    SplayTree(): root_(nullptr) {}
    ~SplayTree() { clear(); }

    // 插入
    bool insert(const T& data) { return _insert(data); }
    bool insert(T&& data) { return _insert(std::move(data)); }

    // 删除
    bool remove(const T& data);

    // 伸展调整
    void splay(const T& data) { splay(root_, data); }

    // 清除
    void clear() { destroy(root_); }

    // 打印
    void print() const { if (root_) print(root_); }

private:

    void splay(Node*& node, const T& data);

    template <class X>
    bool _insert(X&& x);

    void print(Node* node, Node* parent = nullptr) const;

    // 销毁子树
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
    };
    Node* root_;
};


template <class T>
void SplayTree<T>::splay(Node*& node, const T& data)
{
    if (node == nullptr) return;
    Node head;
    head.left = head.right = nullptr;

    Node* leftTreeMax = &head;
    Node* rightTreeMin = &head;

    while (true)
    {
        if (data < node->data)
        {
            if (node->left && data < node->left->data)
            {
                // 右旋
                Node* lchild = node->left;
                node->left = lchild->right;
                lchild->right = node;
                node = lchild;
            }
            if (node->left == nullptr)
                break;
            // 右连接
            rightTreeMin->left = node;
            rightTreeMin = node;
            node = node->left;
        }
        else if (node->data < data)
        {
            if (node->right && node->right->data < data)
            {
                // 左旋
                Node* rchild = node->right;
                node->right = rchild->left;
                rchild->left = node;
                node = rchild;
            }
            if (node->right == nullptr)
                break;
            // 左连接
            leftTreeMax->right = node;
            leftTreeMax = node;
            node = node->right;
        }
        else
        {
            break;
        }
    }
    // 组合
    leftTreeMax->right = node->left;
    rightTreeMin->left = node->right;
    node->left = head.right;
    node->right = head.left;
}


template <class T>
template <class X>
bool SplayTree<T>::_insert(X&& x)
{
    Node* node = new Node();
    node->data = std::forward<X>(x);

    if (root_ == nullptr)
    {
        node->left = node->right = nullptr;
        root_ = node;
        return true;
    }
    splay(root_, x);
    if (x < root_->data)
    {
        node->left = root_->left; // root左子树小于关键字
        node->right = root_; // root及其右子树都大于关键字
        root_->left = nullptr;
        root_ = node; // 新插入的节点作为root
    }
    else if (root_->data < x)
    {
        node->right = root_->right;
        node->left = root_;
        root_->right = nullptr;
        root_ = node;
    }
    else
    {
        delete node;
        return false;  // 元素重复
    }
    return true;
}


template <class T>
bool SplayTree<T>::remove(const T& data)
{
    if (root_ == nullptr)
        return false;

    splay(root_, data);
    if (data != root_->data)
        return false;

    Node* newRoot;
    if (root_->left == nullptr)
    {
        newRoot = root_->right;
    }
    else
    {
        // 用左子树最大节点作为新的根节点
        newRoot = root_->left;
        splay(newRoot, data);
        newRoot->right = root_->right;
    }
    delete root_;
    root_ = newRoot;
    return true;
}


template <class T>
void SplayTree<T>::print(Node* node, Node* parent) const
{
    if (node == nullptr) return;

    using namespace std;
    if (node == root_)
        cout << "root: " << node->data << endl;
    else if (node == parent->left)
        cout << parent->data << " left: " << node->data << endl;
    else
        cout << parent->data << " right: " << node->data << endl;

    print(node->left, node);
    print(node->right, node);
}


// 测试
#include <cstdlib>
#include <ctime>
#include <vector>
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

    SplayTree<int> tree;
    for (const auto& x : vec) tree.insert(x);

    cout << "tree traversal:" << endl;
    tree.print();
    cout << endl;

    cout << "splay " << vec[0] << endl;
    tree.splay(vec[0]);
    cout << "tree traversal:" << endl;
    tree.print();

    for (const auto& x : vec) tree.remove(x);

    return 0;
}
