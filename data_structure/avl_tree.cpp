//AVL树
#include <vector>
#include <queue>


template <class T>
class AVLTree
{

    class Node;
public:

    AVLTree(): root_(nullptr) {}

    AVLTree(const T* begin, const T* end): root_(nullptr)
    { insertRange(begin, end); }

    ~AVLTree() { clear(); }

    //批量插入
    void insertRange(const T* begin, const T* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(*p);
    }


    //插入节点（递归）
    bool insert(const T& x) { return _insert(root_, x); }
    bool insert(T&& x) { return _insert(root_, std::move(x)); }

    bool deleteData(const T& x) { return _deleteData(root_, x); }


    Node* find(const T& data)
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


    std::vector<T> inorder() const
    {
        std::vector<T> res;
        inorder(root_, res);
        return res;
    }

    //层序遍历
    std::vector<T> levelOrder() const
    {
        if (root_ == nullptr) return {};
        std::vector<T> res;
        std::queue<Node*> nodes;
        nodes.push(root_);
        while (!nodes.empty())
        {
            Node* cur = nodes.front();
            nodes.pop();
            res.push_back(cur->data);
            if (cur->left) nodes.push(cur->left);
            if (cur->right) nodes.push(cur->right);
        }
        return res;
    }

    void clear()
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


private:
    template <class X>
    bool _insert(Node*& node, X&& x)
    {
        if (node == nullptr)
        {
            node = new Node(std::forward<X>(x));
            node->left = node->right = nullptr;
        }
        else if (x < node->data)
        {
            if (!_insert(node->left, std::forward<X>(x)))
                return false;
            if (height(node->left) - height(node->right) == 2)
            {
                if (x < node->left->data)
                    rightRotation(node);
                else
                    leftRightRotation(node);
            }
        }
        else if (node->data < x)
        {
            if (!_insert(node->right, std::forward<X>(x)))
                return false;
            if (height(node->right) - height(node->left) == 2)
            {
                if (node->right->data < x)
                    leftRotation(node);
                else
                    rightLeftRotation(node);
            }
        }
        else
        {
            return false;
        }
        updateHeight(node);
        return true;
    }


    bool _deleteData(Node*& node, const T& data)
    {
        if (node == nullptr)
            return false;//未找到
        if (data < node->data)
        {
            if (!_deleteData(node->left, data))
                return false;
            if (height(node->right) - height(node->left) == 2)
            {
                Node* rchild = node->right;
                if (height(rchild->right) >= height(rchild->left))
                    leftRotation(node);
                else
                    rightLeftRotation(node);
            }
        }
        else if (node->data < data)
        {
            if (!_deleteData(node->right, data))
                return false;
            if (height(node->left) - height(node->right) == 2)
            {
                Node* lchild = node->left;
                if (height(lchild->left) >= height(lchild->right))
                    rightRotation(node);
                else
                    leftRightRotation(node);
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
                _deleteData(node->right, data);
            }
            else
            {
                Node* tmp = node;
                node = (node->left != nullptr) ? node->left : node->right;
                delete tmp;
                return true;
            }
        }
        updateHeight(node);
        return true;
    }


    int height(Node* node) const
    {
        return node != nullptr ? node->height : 0;
    }

    void updateHeight(Node* node)
    {
        node->height = std::max(height(node->left), height(node->right)) + 1;
    }

    void leftRotation(Node*& node)
    {
        Node* rchild = node->right;
        node->right = rchild->left;
        rchild->left = node;

        updateHeight(node);
        updateHeight(rchild);

        node = rchild;
    }

    void rightRotation(Node*& node)
    {
        Node* lchild = node->left;
        node->left = lchild->right;
        lchild->right = node;

        updateHeight(node);
        updateHeight(lchild);

        node = lchild;
    }

    void rightLeftRotation(Node*& node)
    {
        rightRotation(node->right);
        leftRotation(node);
    }

    void leftRightRotation(Node*& node)
    {
        leftRotation(node->left);
        rightRotation(node);
    }

    //中序遍历（递归）
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
        int height;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* root_;

};


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

    AVLTree<int> tree(&*vec.cbegin(), &*vec.cend());
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

