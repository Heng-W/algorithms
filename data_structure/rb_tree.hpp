#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <functional>

//红黑树
template <class Object, class Key = Object,
          class ExtractKey = std::_Identity<Object>,
          class Compare = std::less<Key>>
class RBTree
{
    template <class NodePtr> struct Iterator_;
    struct Node;
public:
    using Iterator = Iterator_<Node*>;
    using ConstIterator = Iterator_<const Node*>;
    using KeyType = Key;


    RBTree(): nodeCount_(0), comp_(Compare())
    {
        nil_ = (Node*)::malloc(sizeof(Node));
        nil_->color = BLACK;
        nil_->left = nil_->right = nil_;
        root_ = nil_;
    }

    RBTree(const Object* begin, const Object* end): RBTree()
    { insertRange(begin, end); }

    ~RBTree() { clear(); ::free(nil_); }

    RBTree(RBTree&& tree): RBTree()
    { swap(tree); }

    RBTree& operator=(RBTree&& tree) noexcept
    {
        if (this != &tree)
        {
            clear();
            swap(tree);
        }
        return *this;
    }

    void swap(RBTree& tree)
    {
        using std::swap;
        swap(this, tree);
    }

    //批量插入（拷贝）
    void insertRange(const Object* begin, const Object* end)
    {
        for (auto p = begin; p != end; ++p)
            insert(*p);
    }

    //查找
    ConstIterator find(const KeyType& key) const
    { return ConstIterator(_find(key), this); }

    Iterator find(const KeyType& key)
    { return Iterator(const_cast<Node*>(_find(key)), this); }

    //插入
    std::pair<Iterator, bool>
    insert(const Object& obj) { return _insert(obj); }

    std::pair<Iterator, bool>
    insert(Object&& obj) { return _insert(std::move(obj)); }

    //插入（元素可以重复）
    Iterator insertEqual(const Object& obj) { return _insertEqual(obj); }
    Iterator insertEqual(Object&& obj) { return _insertEqual(std::move(obj)); }

    //删除
    bool remove(const KeyType& key);

    Object& findOrInsert(const Object& obj) { return *insert(obj).first; }
    Object& findOrInsert(Object&& obj) { return *insert(std::move(obj)).first; }


    void clear() { deleteTree(root_); }

    int count() const { return nodeCount_; }

    ConstIterator begin() const { return ConstIterator(minimum(), this); }
    Iterator begin() { return Iterator(const_cast<Node*>(minimum()), this); }

    ConstIterator end() const { return ConstIterator(nil_, this); }
    Iterator end() { return Iterator(nil_, this); }


    const Node* minimum() const
    {
        const Node* cur = root_;
        while (cur->left != nil_) cur = cur->left;
        return cur;
    }

    const Node* maximum() const
    {
        const Node* cur = root_;
        while (cur->right != nil_) cur = cur->right;
        return cur;
    }

private:
    const Node* _find(const KeyType& key) const;

    template <class X>
    std::pair<Iterator, bool> _insert(X&& x);

    template <class X>
    Iterator _insertEqual(X&& x);

    //插入平衡调整
    void insertRebalance(Node* cur);

    //删除平衡调整
    void removeRebalance(Node* cur);


    void leftRotation(Node* node);
    void rightRotation(Node* node);

    void setParentPtr(Node* node, Node* child);

    static const KeyType& getKey(const Object& obj)
    { return ExtractKey()(obj);}

    //递归删除子树节点
    void deleteTree(Node*& node)
    {
        if (node != nil_)
        {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
            node = nil_;
        }
    }

    template <class NodePtr>
    struct Iterator_
    {
        using Self = Iterator_;

        NodePtr node;
        const RBTree* tree;

        using ObjectRef = decltype((node->obj));
        using ObjectPtr = decltype(&node->obj);

        Iterator_() {}
        Iterator_(NodePtr _node, const RBTree* _tree): node(_node), tree(_tree) {}

        bool operator==(const Self& it) const { return node == it.node; }
        bool operator!=(const Self& it) const { return node != it.node; }

        ObjectRef operator*() const { return node->obj; }
        ObjectPtr operator->() const { return &*this; }

        Self& operator++() { increase(); return *this; }
        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self& operator--() { decrease(); return *this; }
        Self operator--(int)
        {
            Self tmp = *this;
            --*this;
            return tmp;
        }
    private:
        void increase();
        void decrease();
    };

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
template <class NodePtr>
void RBTree<Object, KeyType, ExtractKey, Compare>::Iterator_<NodePtr>::
increase()
{
    if (node->right != tree->nil_)
    {
        node = node->right;
        while (node->left != tree->nil_)
            node = node->left;
    }
    else
    {
        while (node == node->parent->right)
            node = node->parent;
        node = node->parent;
    }
}


template <class Object, class KeyType, class ExtractKey, class Compare>
template <class NodePtr>
void RBTree<Object, KeyType, ExtractKey, Compare>::Iterator_<NodePtr>::
decrease()
{
    if (node->left != tree->nil_)
    {
        node = node->left;
        while (node->right != tree->nil_)
            node = node->right;
    }
    else
    {
        while (node == node->parent->left)
            node = node->parent;
        node = node->parent;
    }
}


template <class Object, class KeyType, class ExtractKey, class Compare>
auto RBTree<Object, KeyType, ExtractKey, Compare>::
_find(const KeyType& key) const ->  const Node*
{
    const Node* cur = root_;
    while (cur != nil_)
    {
        if (comp_(key, getKey(cur->obj)))
            cur = cur->left;
        else if (comp_(getKey(cur->obj), key))
            cur = cur->right;
        else
            break;
    }
    return cur;
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
_insert(X&& x) -> std::pair<Iterator, bool>
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
            return {Iterator(cur, this), false};
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
    return {Iterator(node, this), true};
}


template <class Object, class KeyType, class ExtractKey, class Compare>
template <class X>
auto RBTree<Object, KeyType, ExtractKey, Compare>::
_insertEqual(X&& x) -> Iterator
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
    return Iterator(node, this);
}


template <class Object, class KeyType, class ExtractKey, class Compare>
bool  RBTree<Object, KeyType, ExtractKey, Compare>::
remove(const KeyType& key)
{
    Node* node = find(key).node;
    if (node == nil_) return false;
    if (node->left != nil_ && node->right != nil_)
    {
        Node* sub = node->right;
        while (sub->left != nil_)
            sub = sub->left;
        node->obj.~Object();
        new (&node->obj) Object(std::move(sub->obj));
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
