
#include <iostream>

// AVL树
template <class T>
class AVLTree
{
    struct Node;
public:
    AVLTree(): root_(nullptr) {}
    ~AVLTree() { clear(); }

    // 拷贝构造函数
    AVLTree(const AVLTree& rhs) { root_ = clone(rhs.root_); }

    // 移动构造函数
    AVLTree(AVLTree&& rhs) noexcept
    :root_(rhs.root_) { rhs.root_ = nullptr; }

    // 拷贝赋值运算符
    AVLTree& operator=(const AVLTree& rhs)
    {
        Node* newRoot = clone(rhs.root_);
        clear();
        root_ = newRoot;
        return *this;
    }

    // 移动赋值运算符
    AVLTree& operator=(AVLTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            rhs.root_ = nullptr;
        }
        return *this;
    }

    // 查找
    const Node* find(const T& data) const { return _find(data); }
    Node* find(const T& data) { return const_cast<Node*>(_find(data)); }

    // 插入
    bool insert(const T& data) { return _insert(root_, data); }
    bool insert(T&& data) { return _insert(root_, std::move(data)); }

    // 删除
    bool remove(const T& data) { return _remove(root_, data); }

    // 中序遍历
    void inOrder() const { inOrder(root_); }

    // 清除
    void clear() { destroy(root_); }

private:
    const Node* _find(const T& data) const;

    template <class X>
    bool _insert(Node*& node, X&& x);

    bool _remove(Node*& node, const T& data);

    // 获取节点高度
    static int height(const Node* node)
    { return node != nullptr ? node->height : 0; }

    // 更新节点高度
    static void updateHeight(Node* node)
    { node->height = std::max(height(node->left), height(node->right)) + 1; }

    // 左旋
    void leftRotation(Node*& node)
    {
        Node* rchild = node->right;
        node->right = rchild->left;
        rchild->left = node;

        updateHeight(node);
        updateHeight(rchild);

        node = rchild;
    }

    // 右旋
    void rightRotation(Node*& node)
    {
        Node* lchild = node->left;
        node->left = lchild->right;
        lchild->right = node;

        updateHeight(node);
        updateHeight(lchild);

        node = lchild;
    }

    // 中序遍历
    void inOrder(Node* node) const
    {
        if (node)
        {
            inOrder(node->left);
            std::cout << node->data << " ";
            inOrder(node->right);
        }
    }

    // 销毁
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

    // 克隆
    Node* clone(Node* node)
    {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->data);
        copy->height = node->height;
        copy->left = clone(node->left);
        copy->right = clone(node->right);
        return copy;
    }

    // 定义节点
    struct Node
    {
        T data;
        Node* left;
        Node* right;
        int height; // 高度

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* root_; // 根节点
};


template <class T>
auto AVLTree<T>::_find(const T& data) const -> const Node*
{
    const Node* cur = root_;
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
template <class X>
bool AVLTree<T>::_insert(Node*& node, X&& x)
{
    if (node == nullptr) // 插入元素
    {
        node = new Node(std::forward<X>(x));
        node->left = node->right = nullptr;
    }
    else if (x < node->data) // 进入左子树
    {
        if (!_insert(node->left, std::forward<X>(x)))
            return false; // 未找到
        if (height(node->left) - height(node->right) == 2)
        {
            if (node->left->data < x)
                leftRotation(node->left);
            rightRotation(node);
        }
    }
    else if (node->data < x) // 进入右子树
    {
        if (!_insert(node->right, std::forward<X>(x)))
            return false; // 未找到
        if (height(node->right) - height(node->left) == 2)
        {
            if (x < node->right->data)
                rightRotation(node->right);
            leftRotation(node);
        }
    }
    else
    {
        return false;
    }
    updateHeight(node); // 更新高度
    return true;
}


template <class T>
bool AVLTree<T>::_remove(Node*& node, const T& data)
{
    if (node == nullptr)
        return false; // 未找到
    if (data < node->data) // 进入左子树
    {
        if (!_remove(node->left, data))
            return false;
        if (height(node->right) - height(node->left) == 2)
        {
            if (height(node->right->right) < height(node->right->left))
                rightRotation(node->right);
            leftRotation(node);
        }
    }
    else if (node->data < data) // 进入右子树
    {
        if (!_remove(node->right, data))
            return false;
        if (height(node->left) - height(node->right) == 2)
        {
            if (height(node->left->left) < height(node->left->right))
                leftRotation(node->left);
            rightRotation(node);
        }
    }
    else
    {
        if (node->left && node->right)
        {
            Node* sub = node->right;
            while (sub->left)
                sub = sub->left;
            using std::swap;
            swap(node->data, sub->data);
            _remove(node->right, data);
        }
        else
        {
            Node* tmp = node;
            node = (node->left != nullptr) ? node->left : node->right;
            delete tmp;
            return true;
        }
    }
    updateHeight(node); // 更新高度
    return true;
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

    AVLTree<int> tree;
    for (const auto& x : vec) tree.insert(x);

    cout << (tree.find(vec[0]) != nullptr) << endl;
    cout << (tree.find(100) != nullptr) << endl;

    tree.inOrder();
    cout << endl;

    for (const auto& x : vec)
    {
        tree.remove(x);
        tree.inOrder();
        cout << endl;
    }
    return 0;
}

