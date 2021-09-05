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

    RBTree(const Object* begin, const Object* end): RBTree()
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

    //查找
    Node* find(const KeyType& key);

    //插入
    std::pair<Node*, bool>
    insert(const Object& obj) { return _insert(obj); }

    std::pair<Node*, bool>
    insert(Object&& obj) { return _insert(std::move(obj)); }


    //插入（元素可以重复）
    Node* insertEqual(const Object& obj) { return _insert(obj); }
    Node* insertEqual(Object&& obj) { return _insert(std::move(obj)); }

    //删除
    bool remove(const KeyType& key);

    //前序遍历
    std::vector<Object> preorder() const
    {
        std::vector<Object> res;
        preorder(root_, res);
        return res;
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
    std::pair<Node*, bool> _insert(X&& x);

    template <class X>
    Node* _insertEqual(X&& x);

    //插入平衡调整
    void insertRebalance(Node* cur);

    //删除平衡调整
    void removeRebalance(Node* cur);


    void leftRotation(Node* node);
    void rightRotation(Node* node);

    void setParentPtr(Node* node, Node* child);


    static const KeyType&
    getKey(const Object& obj) { return ExtractKey()(obj);}


    //前序遍历
    void preorder(Node* node, std::vector<Object>& res) const
    {
        if (node != nil_)
        {
            res.push_back(node->obj);
            preorder(node->left, res);
            preorder(node->right, res);
        }
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


template <class Object, class KeyType, class ExtractKey, class Compare>
auto RBTree<Object, KeyType, ExtractKey, Compare>::
find(const KeyType& key) ->  Node*
{
    Node* cur = root_;
    while (cur != nil_)
    {
        if (comp_(key, getKey(cur->obj)))
            cur = cur->left;
        else if (comp_(getKey(cur->obj), key))
            cur = cur->right;
        else
            return cur;
    }
    return nullptr;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
inline void RBTree<Object, KeyType, ExtractKey, Compare>::
setParentPtr(Node* node, Node* child)
{
    child->parent = node->parent;
    if (node == root_)
        root_ = child;
    else if (node == node->parent->left)
        node->parent->left = child;
    else
        node->parent->right = child;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
inline void RBTree<Object, KeyType, ExtractKey, Compare>::
leftRotation(Node* node)
{
    Node* rchild = node->right;
    node->right = rchild->left;
    if (rchild->left != nil_)
        rchild->left->parent = node;

    setParentPtr(node, rchild);

    rchild->left = node;
    node->parent = rchild;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
inline void RBTree<Object, KeyType, ExtractKey, Compare>::
rightRotation(Node* node)
{
    Node* lchild = node->left;
    node->left = lchild->right;
    if (lchild->right != nil_)
        lchild->right->parent = node;

    setParentPtr(node, lchild);

    lchild->right = node;
    node->parent = lchild;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
template <class X>
auto RBTree<Object, KeyType, ExtractKey, Compare>::
_insert(X&& x) -> std::pair<Node*, bool>
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


template <class Object, class KeyType, class ExtractKey, class Compare>
template <class X>
auto RBTree<Object, KeyType, ExtractKey, Compare>::
_insertEqual(X&& x) -> Node*
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
    insertRebalance(node);
    return node;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
bool  RBTree<Object, KeyType, ExtractKey, Compare>::
remove(const KeyType& key)
{
    Node* node = find(key);
    if (node == nullptr) return false;
    if (node->left != nil_ && node->right != nil_)
    {
        Node* sub = node->right;
        while (sub->left != nil_)
            sub = sub->left;
        node->obj = std::move(sub->obj);
        node = sub;
    }
    if (node->left == nil_ && node->right == nil_)
    {
        setParentPtr(node, nil_);
        if (node->color == BLACK)
            removeRebalance(nil_);
    }
    else
    {
        Node* child = (node->left != nil_) ? node->left : node->right;
        setParentPtr(node, child);
        child->color = BLACK;
    }
    delete node;
    --nodeCount_;
    return true;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
void  RBTree<Object, KeyType, ExtractKey, Compare>::
insertRebalance(Node* cur)
{
    while (cur->parent->color == RED)
    {
        if (cur->parent == cur->parent->parent->left)
        {
            Node* uncle = cur->parent->parent->right; //伯父节点
            if (uncle->color == RED)
            {
                cur->parent->color = BLACK;
                uncle->color = BLACK;
                cur->parent->parent->color = RED;
                cur = cur->parent->parent;
            }
            else
            {
                if (cur == cur->parent->right)
                {
                    cur = cur->parent;
                    leftRotation(cur);
                }
                cur->parent->color = BLACK;
                cur->parent->parent->color = RED;
                rightRotation(cur->parent->parent);
            }
        }
        else
        {
            Node* uncle = cur->parent->parent->left; //伯父节点
            if (uncle->color == RED)
            {
                cur->parent->color = BLACK;
                uncle->color = BLACK;
                cur->parent->parent->color = RED;
                cur = cur->parent->parent;
            }
            else
            {
                if (cur == cur->parent->left)
                {
                    cur = cur->parent;
                    rightRotation(cur);
                }
                cur->parent->color = BLACK;
                cur->parent->parent->color = RED;
                leftRotation(cur->parent->parent);
            }
        }
    }
    root_->color = BLACK;
}


template <class Object, class KeyType, class ExtractKey, class Compare>
void RBTree<Object, KeyType, ExtractKey, Compare>::
removeRebalance(Node* cur)
{
    while (cur != root_)
    {
        if (cur == cur->parent->left) //当前平衡点为左子
        {
            Node* brother = cur->parent->right;
            if (brother->color == RED)
            {
                brother->color = BLACK;
                cur->parent->color = RED;
                leftRotation(cur->parent);
                brother = cur->parent->right;
            }
            if (brother->left->color == BLACK && brother->right->color == BLACK)
            {
                brother->color = RED;
                if (cur->parent->color == RED)
                {
                    cur->parent->color = BLACK;
                    return;
                }
                else
                {
                    cur = cur->parent;
                }
            }
            else
            {
                if (brother->right->color == BLACK)
                {
                    brother->color = RED;
                    brother->left->color = BLACK;
                    rightRotation(brother);
                    brother = cur->parent->right;
                }
                brother->color = cur->parent->color;
                cur->parent->color = BLACK;
                brother->right->color = BLACK;
                leftRotation(cur->parent);
                return;
            }
        }
        else  //当前平衡点为右子
        {
            Node* brother = cur->parent->left;
            if (brother->color == RED)
            {
                brother->color = BLACK;
                cur->parent->color = RED;
                rightRotation(cur->parent);
                brother = cur->parent->left;
            }
            if (brother->left->color == BLACK && brother->right->color == BLACK)
            {
                brother->color = RED;
                if (cur->parent->color == RED)
                {
                    cur->parent->color = BLACK;
                    return;
                }
                else
                {
                    cur = cur->parent;
                }
            }
            else
            {
                if (brother->left->color == BLACK)
                {
                    brother->color = RED;
                    brother->right->color = BLACK;
                    leftRotation(brother);
                    brother = cur->parent->left;
                }
                brother->color = cur->parent->color;
                cur->parent->color = BLACK;
                brother->left->color = BLACK;
                rightRotation(cur->parent);
                return;
            }
        }
    }
}


#endif //RB_TREE_HPP
