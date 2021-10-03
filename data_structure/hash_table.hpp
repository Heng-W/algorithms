#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <functional>
#include <vector>

//哈希表
template <class Object, class HashFunc = std::hash<Object>,
          class ExtractKey = std::_Identity<Object>>
class HashTable
{
    template <class NodePtr> struct Iterator_;
    struct Node;
public:
    using Iterator = Iterator_<Node*>;
    using ConstIterator = Iterator_<const Node*>;
    using KeyType = typename std::result_of<ExtractKey(Object)>::type;

    HashTable(int n = 32): nodeCount_(0) { initBuckets(n); }
    ~HashTable() { clear(); }

    //拷贝构造函数
    HashTable(const HashTable& rhs);

    //移动构造函数
    HashTable(HashTable&& rhs) noexcept
        : buckets_(std::move(rhs.buckets_)),
          nodeCount_(rhs.nodeCount_)
    { rhs.nodeCount_ = 0; }

    //拷贝赋值运算符
    HashTable& operator=(const HashTable& rhs)
    {
        HashTable copy = rhs;
        return *this = std::move(copy);
    }

    //移动赋值运算符
    HashTable& operator=(HashTable&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            buckets_ = std::move(rhs.buckets_);
            nodeCount_ = rhs.nodeCount_;
            rhs.nodeCount_ = 0;
        }
        return *this;
    }

    //插入元素（不重复）
    std::pair<Iterator, bool> insert(const Object& obj)
    { return _insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return _insert(std::move(obj)); }

    //插入元素（可以重复）
    Iterator insertEqual(const Object& obj) { return _insertEqual(obj); }
    Iterator insertEqual(Object&& obj) { return _insertEqual(std::move(obj)); }

    //查找
    ConstIterator find(const KeyType& key) const
    { return ConstIterator(_find(key), this); }

    Iterator find(const KeyType& key)
    { return Iterator(const_cast<Node*>(_find(key)), this); }

    //查找（不存在则插入）
    Object& findOrInsert(const Object& obj)
    { return *insert(obj).first; }

    Object& findOrInsert(Object&& obj)
    { return *insert(std::move(obj)).first; }

    bool remove(const KeyType& key);

    void clear();


    int nodeCount() const { return nodeCount_; }

    int bucketCount() const { return buckets_.size(); }


    ConstIterator begin() const { return ConstIterator(_begin(), this); }
    Iterator begin() { return Iterator(const_cast<Node*>(_begin()), this); }

    ConstIterator end() const { return ConstIterator(nullptr, this); }
    Iterator end() { return Iterator(nullptr, this); }

private:

    const Node* _find(const KeyType& key) const;

    template <class X>
    std::pair<Iterator, bool> _insert(X&& obj);

    template <class X>
    Iterator _insertEqual(X&& obj);

    void resize(int hintCnt);

    void initBuckets(int size)
    {
        int newSize = roundup(size);
        buckets_.resize(newSize, nullptr);
        nodeCount_ = 0;
    }

    const Node* _begin() const
    {
        for (int i = 0; i < buckets_.size(); ++i)
        {
            if (buckets_[i]) return buckets_[i];
        }
        return nullptr;
    }

    int bucketPos(const KeyType& key) const
    { return bucketPos(key, buckets_.size()); }

    int bucketPos(const KeyType& key, int n) const
    { return HashFunc()(key) % n; }

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

    static const KeyType& getKey(const Object& obj)
    { return ExtractKey()(obj); }


    template <class NodePtr>
    struct Iterator_
    {
        NodePtr node;
        const HashTable* tab;

        using Self = Iterator_;
        using ObjectRef = decltype((node->obj));
        using ObjectPtr = decltype(&node->obj);

        Iterator_() {}
        Iterator_(NodePtr _node, const HashTable* _tab): node(_node), tab(_tab) {}

        bool operator==(const Self& it) const { return node == it.node; }
        bool operator!=(const Self& it) const { return node != it.node; }

        ObjectRef operator*() const { return node->obj; }
        ObjectPtr operator->() const { return &*this; }

        Self& operator++()
        {
            const Node* old = node;
            node = node->next;
            if (!node)
            {
                //根据元素值，定位出下一个bucket
                int pos = tab->bucketPos(tab->getKey(old->obj)) + 1;
                while (pos < tab->buckets_.size() && !tab->buckets_[pos]) ++pos;
                if (pos < tab->buckets_.size())
                    node = tab->buckets_[pos];
            }
            return *this;
        }

        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }
    };

    struct Node
    {
        Node* next;
        Object obj;

        Node(const Object& _obj): obj(_obj) {}
        Node(Object&& _obj): obj(std::move(_obj)) {}
    };

    std::vector<Node*> buckets_;
    int nodeCount_;
};


template <class Object, class HashFunc, class ExtractKey>
auto HashTable<Object, HashFunc, ExtractKey>::
_find(const KeyType& key) const -> const Node*
{
    int pos = bucketPos(key);//找到位置
    const Node* cur = buckets_[pos];
    while (cur)
    {
        if (key == getKey(cur->obj))
            return cur;
        cur = cur->next;
    }
    return nullptr;
}


template <class Object, class HashFunc, class ExtractKey>
template <class X>
auto HashTable<Object, HashFunc, ExtractKey>::
_insert(X&& obj) -> std::pair<Iterator, bool>
{
    resize(nodeCount_ + 1);//检查是否需要重建表格

    int pos = bucketPos(getKey(obj));//找到位置
    Node* cur = buckets_[pos];
    while (cur)
    {
        if (getKey(obj) == getKey(cur->obj))
            return {Iterator(cur, this), false};
        cur = cur->next;
    }
    Node* node = new Node(std::forward<X>(obj));
    node->next = buckets_[pos];
    buckets_[pos] = node;
    ++nodeCount_;
    return {Iterator(node, this), true};
}


template <class Object, class HashFunc, class ExtractKey>
template <class X>
auto HashTable<Object, HashFunc, ExtractKey>::
_insertEqual(X&& obj) -> Iterator
{
    resize(nodeCount_ + 1);//检查是否需要重建表格

    Node* node = new Node(std::forward<X>(obj));
    int pos = bucketPos(getKey(obj));//找到位置
    Node* cur = buckets_[pos];
    while (cur)
    {
        if (getKey(obj) == getKey(cur->obj))
        {
            node->next = cur->next;
            cur->next = node;
            ++nodeCount_;
            return node;
        }
        cur = cur->next;
    }
    node->next = buckets_[pos];
    buckets_[pos] = node;
    ++nodeCount_;
    return Iterator(node, this);
}


template <class Object, class HashFunc, class ExtractKey>
bool HashTable<Object, HashFunc, ExtractKey>::
remove(const KeyType& key)
{
    int pos = bucketPos(key);//找到位置
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
void HashTable<Object, HashFunc, ExtractKey>::resize(int hintCnt)
{
    if (hintCnt <= buckets_.size())
        return;
    int newSize = roundup(hintCnt);
    std::vector<Node*> tmp(newSize, nullptr);

    for (int i = 0; i < buckets_.size(); ++i)
    {
        Node* first = buckets_[i];
        while (first)
        {
            //找到在新buckets中的位置
            int newPos = bucketPos(getKey(first->obj), newSize);
            //旧bucket指向下一个节点
            buckets_[i] = first->next;
            //当前节点插入到新bucket
            first->next = tmp[newPos];
            tmp[newPos] = first;
            //准备处理旧bucket的下一个节点
            first = buckets_[i];
        }
    }
    buckets_.swap(tmp);
}


template <class Object, class HashFunc, class ExtractKey>
void HashTable<Object, HashFunc, ExtractKey>::clear()
{
    for (int i = 0; i < buckets_.size(); ++i)
    {
        Node* cur = buckets_[i];
        while (cur)
        {
            Node* next = cur->next;
            delete cur;
            cur = next;
        }
        buckets_[i] = nullptr;
    }
    buckets_.clear();
    nodeCount_ = 0;
}


template <class Object, class HashFunc, class ExtractKey>
HashTable<Object, HashFunc, ExtractKey>::
HashTable(const HashTable& rhs): nodeCount_(rhs.nodeCount_)
{
    buckets_.resize(rhs.buckets_.size());
    for (int i = 0; i < rhs.buckets_.size(); ++i)
    {
        if (rhs.buckets_[i])
        {
            const Node* cur = rhs.buckets_[i];
            Node* copy = new Node(cur->obj);
            buckets_[i] = copy;

            while (cur->next)
            {
                copy->next = new Node(cur->next->obj);
                cur = cur->next;
                copy = copy->next;
            }
            copy->next = nullptr;
        }
    }
};


#endif //HASH_TABLE_HPP
