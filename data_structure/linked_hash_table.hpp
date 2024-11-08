#ifndef LINKED_HASH_TABLE_HPP
#define LINKED_HASH_TABLE_HPP

#include <assert.h>
#include <cstdlib>
#include <functional>
#include <vector>

// 哈希表和双向循环链表结合
template <class Object, class HashFunc = std::hash<Object>,
          class ExtractKey = std::_Identity<Object>>
class LinkedHashTable
{
    template <class NodePtr> struct IteratorT;
    struct Node;
public:
    using Iterator = IteratorT<Node*>;
    using ConstIterator = IteratorT<const Node*>;
    using KeyType = typename std::result_of<ExtractKey(Object)>::type;

    using RemoveCallback = std::function<bool()>;

    LinkedHashTable(int n = 32): nodeCount_(0)
    {
        buckets_.resize(roundup(n));
        head_ = (Node*)::malloc(sizeof(Node));
        head_->after = head_;
        head_->before = head_;
    }

    ~LinkedHashTable() { clear(); ::free(head_); }

    // 拷贝构造函数
    LinkedHashTable(const LinkedHashTable& rhs)
        : LinkedHashTable(rhs.buckets_.size())
    {
        for (const auto& x : rhs) _insert(bucketPos(getKey(x)), x);
        nodeCount_ = rhs.nodeCount_;
        removeCallback_ = rhs.removeCallback_;
    }

    // 移动构造函数
    LinkedHashTable(LinkedHashTable&& rhs): LinkedHashTable(0)
    { swap(rhs); }

    // 拷贝赋值运算符
    LinkedHashTable& operator=(const LinkedHashTable& rhs)
    {
        LinkedHashTable copy = rhs;
        swap(copy);
        return *this;
    }

    // 移动赋值运算符
    LinkedHashTable& operator=(LinkedHashTable&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    void swap(LinkedHashTable& rhs)
    {
        using std::swap;
        buckets_.swap(rhs.buckets_);
        swap(head_, rhs.head_);
        swap(nodeCount_, rhs.nodeCount_);
        swap(removeCallback_, rhs.removeCallback_);
    }

    void setRemoveCallback(const RemoveCallback& cb)
    { removeCallback_ = cb; }

    // 插入
    std::pair<Iterator, bool> insert(const Object& obj)
    { return _insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return _insert(std::move(obj)); }

    // 查找
    ConstIterator find(const KeyType& key) const
    { return _find(key); }

    Iterator find(const KeyType& key)
    { return const_cast<Node*>(_find(key)); }

    // 查找（未找到则插入）
    Object& findOrInsert(const Object& obj)
    { return *insert(obj).first; }

    Object& findOrInsert(Object&& obj)
    { return *insert(std::move(obj)).first; }

    // 删除
    void removeFirst() { erase(begin()); }
    Iterator erase(Iterator it);
    bool remove(const KeyType& key);

    void clear();

    int size() const { return nodeCount_; }
    int bucketCount() const { return buckets_.size(); }

    ConstIterator begin() const { return head_->after; }
    Iterator begin() { return head_->after; }

    ConstIterator end() const { return head_; }
    Iterator end() { return head_; }

private:

    const Node* _find(const KeyType& key) const;

    template <class X>
    std::pair<Iterator, bool> _insert(X&& obj);

    template <class X>
    Node* _insert(int pos, X&& obj);

    int bucketPos(const KeyType& key) const
    { return bucketPos(key, buckets_.size()); }

    int bucketPos(const KeyType& key, int n) const
    { return HashFunc()(key) % n; }

    static const KeyType& getKey(const Object& obj)
    { return ExtractKey()(obj); }

    static int roundup(int n)
    { return roundupPowerOfTwo(n); }

    static unsigned int roundupPowerOfTwo(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i + 1;
    }

    void resize(int hintCnt);

    // 迭代器
    template <class NodePtr>
    struct IteratorT
    {
        NodePtr node;

        using Self = IteratorT;
        using ObjectRef = decltype((node->obj));
        using ObjectPtr = decltype(&node->obj);

        IteratorT() {}
        IteratorT(NodePtr _node): node(_node) {}

        bool operator==(const Self& it) const { return node == it.node; }
        bool operator!=(const Self& it) const { return node != it.node; }

        ObjectRef operator*() const { return node->obj; }
        ObjectPtr operator->() const { return &*this; }

        Self& operator++() { node = node->after; return *this; }
        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }

        Self& operator--() { node = node->before; return *this; }
        Self operator--(int)
        {
            Self tmp = *this;
            --*this;
            return tmp;
        }
    };

    struct Node
    {
        Object obj;
        Node* next;
        Node* before;
        Node* after;

        Node(const Object& _obj): obj(_obj) {}
        Node(Object&& _obj): obj(std::move(_obj)) {}
    };

    std::vector<Node*> buckets_; // 桶
    Node* head_; // 头结点
    int nodeCount_;
    RemoveCallback removeCallback_;
};


template <class Object, class HashFunc, class ExtractKey>
auto LinkedHashTable<Object, HashFunc, ExtractKey>::
_find(const KeyType& key) const -> const Node*
{
    int pos = bucketPos(key); // 找到位置
    const Node* cur = buckets_[pos];
    while (cur)
    {
        if (key == getKey(cur->obj)) return cur;
        cur = cur->next;
    }
    return head_;
}


template <class Object, class HashFunc, class ExtractKey>
template <class X>
inline auto LinkedHashTable<Object, HashFunc, ExtractKey>::
_insert(int pos, X&& obj) -> Node*
{
    Node* node = new Node(std::forward<X>(obj));
    node->next = buckets_[pos];
    buckets_[pos] = node;

    node->after = head_;
    node->before = head_->before;
    head_->before->after = node;
    head_->before = node;
    return node;
}

template <class Object, class HashFunc, class ExtractKey>
template <class X>
auto LinkedHashTable<Object, HashFunc, ExtractKey>::
_insert(X&& obj) -> std::pair<Iterator, bool>
{
    resize(nodeCount_ + 1); // 检查是否需要重建表格

    int pos = bucketPos(getKey(obj)); // 找到位置
    Node* cur = buckets_[pos];
    while (cur)
    {
        if (getKey(obj) == getKey(cur->obj))
        {
            return {cur, false};
        }
        cur = cur->next;
    }
    Node* node = _insert(pos, std::forward<X>(obj));
    ++nodeCount_;
    if (removeCallback_ && removeCallback_()) removeFirst();
    return {node, true};
}


template <class Object, class HashFunc, class ExtractKey>
bool LinkedHashTable<Object, HashFunc, ExtractKey>::
remove(const KeyType& key)
{
    int pos = bucketPos(key); // 找到位置
    Node* cur = buckets_[pos];
    Node* prev = nullptr;
    while (cur)
    {
        if (key == getKey(cur->obj))
        {
            if (prev == nullptr)
                buckets_[pos] = cur->next;
            else
                prev->next = cur->next;
                
            cur->before->after = cur->after;
            cur->after->before = cur->before;
            delete cur;
            --nodeCount_;
            return true;
        }
        prev = cur;
        cur = cur->next;
    }
    return false;
}


template <class Object, class HashFunc, class ExtractKey>
auto LinkedHashTable<Object, HashFunc, ExtractKey>::
erase(Iterator it) -> Iterator
{
    int pos = bucketPos(getKey(*it)); // 找到位置
    Node* cur = buckets_[pos];
    Node* prev = nullptr;
    while (cur)
    {
        if (cur == it.node)
        {
            if (prev == nullptr)
                buckets_[pos] = cur->next;
            else
                prev->next = cur->next;
                
            cur->before->after = cur->after;
            cur->after->before = cur->before;
            Node* after = cur->after;
            delete cur;
            --nodeCount_;
            return after;
        }
        prev = cur;
        cur = cur->next;
    }
    abort();
}


template <class Object, class HashFunc, class ExtractKey>
void LinkedHashTable<Object, HashFunc, ExtractKey>::resize(int hintCnt)
{
    if (hintCnt <= (int)buckets_.size()) return;
    int newSize = roundup(hintCnt);
    std::vector<Node*> tmp(newSize, nullptr);

    for (int i = 0; i < (int)buckets_.size(); ++i)
    {
        Node* first = buckets_[i];
        while (first)
        {
            // 找到在新buckets中的位置
            int newPos = bucketPos(getKey(first->obj), newSize);
            // 旧bucket指向下一个节点
            buckets_[i] = first->next;
            // 当前节点插入到新bucket
            first->next = tmp[newPos];
            tmp[newPos] = first;
            // 准备处理旧bucket的下一个节点
            first = buckets_[i];
        }
    }
    buckets_.swap(tmp);
}


template <class Object, class HashFunc, class ExtractKey>
void LinkedHashTable<Object, HashFunc, ExtractKey>::clear()
{
    Node* cur = head_->after;
    while (cur != head_)
    {
        cur = cur->after;
        delete cur->before;
    }
    head_->after = head_;
    head_->before = head_;
    nodeCount_ = 0;
}


#endif // LINKED_HASH_TABLE_HPP
