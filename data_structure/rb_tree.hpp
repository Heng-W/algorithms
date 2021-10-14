#ifndef RB_TREE_HPP
#define RB_TREE_HPP

#include <functional>

//红黑树
//Object：节点存储的对象，Key：键类型
//ExtractKey：对象中提取键，Compare：比较方式
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

    //默认构造函数
    RBTree(): nodeCount_(0)
    {
        nil_ = (Node*)::malloc(sizeof(Node));
        nil_->color = BLACK;
        nil_->left = nil_->right = nil_;
        root_ = nil_;
    }

    //析构函数
    ~RBTree() { clear(); ::free(nil_); }

    //拷贝构造函数
    RBTree(const RBTree& rhs): RBTree()
    { root_ = clone(rhs, rhs.root_, nil_); }

    //移动构造函数
    RBTree(RBTree&& rhs): RBTree() { swap(rhs); }

    //拷贝赋值运算符
    RBTree& operator=(const RBTree& rhs)
    {
        RBTree copy = rhs;
        swap(rhs);
        return *this;
    }

    //移动赋值运算符
    RBTree& operator=(RBTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    //交换
    void swap(RBTree& rhs)
    {
        using std::swap;
        swap(root_, rhs.root_);
        swap(nil_, rhs.nil_);
        swap(nodeCount_, rhs.nodeCount_);
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

    //插入（元素允许重复）
    Iterator insertEqual(const Object& obj) { return _insertEqual(obj); }
    Iterator insertEqual(Object&& obj) { return _insertEqual(std::move(obj)); }

    //删除
    bool remove(const KeyType& key);

    //查找（不存在则插入）
    Object& findOrInsert(const Object& obj) { return *insert(obj).first; }
    Object& findOrInsert(Object&& obj) { return *insert(std::move(obj)).first; }

    //清除
    void clear() { destroy(root_); }

    //元素数量
    int count() const { return nodeCount_; }

    //首尾迭代器
    ConstIterator begin() const { return ConstIterator(minimum(), this); }
    Iterator begin() { return Iterator(const_cast<Node*>(minimum()), this); }

    ConstIterator end() const { return ConstIterator(nil_, this); }
    Iterator end() { return Iterator(nil_, this); }

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

    //左旋，右旋
    void leftRotation(Node* node);
    void rightRotation(Node* node);

    //child接到node的parent节点
    void setParentPtr(Node* node, Node* child);

    static const KeyType& getKey(const Object& obj)
    { return ExtractKey()(obj); }

    static bool comp(const KeyType& key1, const KeyType& key2)
    { return Compare()(key1, key2); }

    //最小值
    const Node* minimum() const
    {
        const Node* cur = root_;
        while (cur->left != nil_) cur = cur->left;
        return cur;
    }

    //销毁
    void destroy(Node*& node)
    {
        if (node != nil_)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nil_;
        }
    }

    //克隆
    Node* clone(const RBTree& tree, Node* cur, Node* parent) const
    {
        if (cur == tree.nil_) return nil_;
        Node* copy = new Node(cur->obj);
        copy->color = cur->color;
        copy->parent = parent;
        copy->left = clone(tree, cur->left, copy);
        copy->right = clone(tree, cur->right, copy);
        return copy;
    }

    //迭代器定义
    template <class NodePtr>
    struct Iterator_
    {
        NodePtr node; //封装的指针
        const RBTree* tree; //用于获取nil节点

        using Self = Iterator_;
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

    //节点定义
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

    Node* root_; //根节点
    Node* nil_; //哨兵节点
    int nodeCount_; //节点个数
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
auto RBTree<Object, KeyType, ExtractKey, Compare>::
_find(const KeyType& key) const ->  const Node*
{
    const Node* cur = root_;
    while (cur != nil_)
    {
        if (comp(key, getKey(cur->obj)))
            cur = cur->left;
        else if (comp(getKey(cur->obj), key))
            cur = cur->right;
        else
            break;
    }
    return cur;
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
        if (comp(getKey(x), getKey(cur->obj)))
            cur = cur->left;
        else if (comp(getKey(cur->obj), getKey(x)))
            cur = cur->right;
        else
            return {Iterator(cur, this), false};
    }
    Node* node = new Node(std::forward<X>(x));
    node->left = node->right = nil_;
    node->parent = parent;
    node->color = RED; //新节点必为红色

    if (root_ == nil_)
        root_ = node;
    else if (comp(getKey(x), getKey(parent->obj)))
        parent->left = node;
    else
        parent->right = node;
    ++nodeCount_;
    insertRebalance(node); //平衡调整
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
        if (comp(getKey(x), getKey(cur->obj)))
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
    else if (comp(getKey(x), getKey(parent->obj)))
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
        if (cur->parent == cur->parent->parent->left) //父节点为祖父节点左子
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
        else //父节点为祖父节点右子，情况对称处理
        {
            Node* uncle = cur->parent->parent->left;
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
    root_->color = BLACK; //根节点始终为黑色
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
            if (brother->color == RED) //兄弟节点为红色，则先旋转调整为黑色
            {
                brother->color = BLACK;
                cur->parent->color = RED;
                leftRotation(cur->parent);
                brother = cur->parent->right;
            }
            if (brother->left->color == BLACK && brother->right->color == BLACK)
            {
                brother->color = RED;
                if (cur->parent->color == RED) //父节点为红色
                {
                    cur->parent->color = BLACK;
                    return; //父节点涂黑后平衡结束
                }
                else //父节点为黑色
                {
                    cur = cur->parent; //继续向上调整
                }
            }
            else //兄弟的子节点不是全黑
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
                return; //平衡结束
            }
        }
        else  //当前平衡点为右子，情况对称处理
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