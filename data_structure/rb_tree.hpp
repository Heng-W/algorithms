#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <cstdlib>
#include <vector>
#include <iostream>


//红黑树
template <class Object, class KeyType = Object,
          class ExtractKey = std::_Identity<Object>,
          class Compare = std::less<KeyType>>
class RBTree
{
    class Node;
public:
    RBTree() : root_(nullptr), nodeCount_(0), comp_(Compare())
    {
        nil_ = (Node*)::malloc(sizeof(Node));
        nil_->color = BLACK;
        root_ = nil_;
    }

    RBTree(const Object* begin, const Object* end)
        : RBTree()
    { insertRange(begin, end); }

    ~RBTree()
    {
        clear();
        ::free(nil_);
    }

    //批量插入（拷贝）
    void insertRange(const Object* begin, const Object* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(*p);
    }


    Node* find(const KeyType& key)
    {
        Node* cur = root_;
        while (cur != nil_)
        {
            if (comp_(key, getKey(cur->obj)))
                cur = cur->left;
            else if (comp_(getKey(cur->obj) , key))
                cur = cur->right;
            else
                return cur;
        }
        return nullptr;
    }


    std::pair<Node*, bool> insert(const Object& obj)
    { return _insert(obj); }

    std::pair<Node*, bool> insert(Object&& obj)
    { return _insert(std::move(obj)); }


    bool remove(const KeyType& key)
    {
        Node* node = find(key);
        if (node == nullptr) return false;
        removeNode(node);
        return true;
    }

    //中序遍历
    std::vector<Object> inorder() const
    {
        std::vector<Object> res;
        inorder(root_, res);
        return res;
    }

    void clear() { deleteTree(root_); }

    int nodeCount() const { return nodeCount_; }

private:

    template <class X>
    std::pair<Node*, bool> _insert(X&& x)
    {
        Node* parent = nil_;
        Node* cur = root_;
        while (cur != nil_)
        {
            parent = cur;
            if (comp_(getKey(x), getKey(cur->obj)))
                cur = cur->left;
            else if (comp_(getKey(cur->obj), getKey(x)))
                cur = cur->right;
            else
                return {cur, false};
        }
        Node* node = new Node(std::forward<X>(x));
        node->left = node->right = nil_;
        node->parent = parent;
        node->color = RED;

        if (root_ == nil_)
            root_ = node;
        else if (comp_(getKey(x), getKey(parent->obj)))
            parent->left = node;
        else
            parent->right = node;
        ++nodeCount_;
        insertRebalance(node);
        return {node, true};
    }

    template <class X>
    Node* _insertEqual(X&& x)
    {
        Node* parent = nil_;
        Node* cur = root_;
        while (cur != nil_)
        {
            parent = cur;
            if (comp_(getKey(x), getKey(cur->obj)))
                cur = cur->left;
            else
                cur = cur->right;
        }
        Node* node = new Node(std::forward<X>(x));
        node->left = node->right = nil_;
        node->parent = parent;
        node->color = RED;

        if (root_ == nil_)
            root_ = node;
        else if (comp_(getKey(x), getKey(parent->obj)))
            parent->left = node;
        else
            parent->right = node;
        ++nodeCount_;
        insertRebalance();
        return node;
    }

    void insertRebalance(Node* x)
    {
        while (x->parent->color == RED)
        {
            if (x->parent == x->parent->parent->left)
            {
                Node* uncle = x->parent->parent->right; //伯父节点
                if (uncle->color == RED)
                {
                    x->parent->color = BLACK;
                    uncle->color = BLACK;
                    x->parent->parent->color = RED;
                    x = x->parent->parent;
                }
                else
                {
                    if (x == x->parent->right)
                    {
                        x = x->parent;
                        leftRotation(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    rightRotation(x->parent->parent);
                }
            }
            else
            {
                Node* uncle = x->parent->parent->left; //伯父节点
                if (uncle->color == RED)
                {
                    x->parent->color = BLACK;
                    uncle->color = BLACK;
                    x->parent->parent->color = RED;
                    x = x->parent->parent;
                }
                else
                {
                    if (x == x->parent->left)
                    {
                        x = x->parent;
                        rightRotation(x);
                    }
                    x->parent->color = BLACK;
                    x->parent->parent->color = RED;
                    leftRotation(x->parent->parent);
                }
            }
        }
        root_->color = BLACK;
    }

    //查找最小结点
    static Node* minimum(Node* node)
    {
        while (node->left) node = node->left;
        return node;
    }

    // 查找最大结点
    static Node* maximum(Node* node)
    {
        while (node->right) node = node->right;
        return node;
    }

    static const KeyType& getKey(const Object& obj)
    {
        return ExtractKey()(obj);
    }


    void leftRotation(Node* node)
    {
        Node* rchild = node->right;
        node->right = rchild->left;
        if (rchild->left != nil_)
            rchild->left->parent = node;

        rchild->parent = node->parent;
        //修改node父节点
        if (node == root_)
            root_ = rchild;
        else if (node == node->parent->left)
            node->parent->left = rchild;
        else
            node->parent->right = rchild;

        rchild->left = node;
        node->parent = rchild;
    }

    void rightRotation(Node* node)
    {
        Node* lchild = node->left;
        node->left = lchild->right;
        if (lchild->right != nil_)
            lchild->right->parent = node;

        lchild->parent = node->parent;
        //修改node父节点
        if (node == root_)
            root_ = lchild;
        else if (node == node->parent->left)
            node->parent->left = lchild;
        else
            node->parent->right = lchild;

        lchild->right = node;
        node->parent = lchild;
    }

    //中序遍历
    void inorder(Node* node, std::vector<Object>& res) const
    {
        if (node != nil_)
        {
            inorder(node->left, res);
            res.push_back(node->obj);
            inorder(node->right, res);
        }
    }

    //递归删除子树节点
    void deleteTree(Node*& node)
    {
        if (node != nil_)
        {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
            node = nullptr;
        }
    }


    enum Color : char {RED, BLACK};

    struct Node
    {
        Object obj;
        Color color;

        Node* left;
        Node* right;
        Node* parent;

        Node(const Object& _obj): obj(_obj) {}
        Node(Object&& _obj): obj(std::move(_obj)) {}
    };

    Node* root_;
    Node* nil_;
    int nodeCount_;
    Compare comp_;

};

#endif //RB_TREE_HPP

