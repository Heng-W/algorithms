#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <iostream>

//哈希表
template <class Object, class HashFunc, class ExtractKey>
class HashTable
{
    friend std::ostream& operator<<(std::ostream& out, const HashTable& ht)
    {
        for (int i = 0; i < ht.buckets_.size(); ++i)
        {
            auto cur = ht.buckets_[i];
            while (cur)
            {
                out << cur->obj << " ";
                cur = cur->next;
            }
        }
        return out;
    }

    class Node;
public:
    using Iterator = Node*;
    using ConstIterator = const Node*;
    using KeyType = typename std::result_of<ExtractKey(Object)>::type;

    HashTable(int n = 32):
        nodeCnt_(0),
        hash_(HashFunc()),
        getKey_(ExtractKey())
    {
        initBuckets(n);
    }

    std::pair<Node*, bool> insertUnique(const Object& obj)
    { return _insertUnique(obj); }

    std::pair<Node*, bool> insertUnique(Object&& obj)
    { return _insertUnique(std::move(obj)); }

    Node* insertEqual(const Object& obj) { return _insertEqual(obj); }
    Node* insertEqual(Object&& obj) { return _insertEqual(std::move(obj)); }

    Node* find(const KeyType& key)
    {
        const auto* tmp = this;
        return const_cast<Node*>(tmp->find(key));
    }

    const Node* find(const KeyType& key) const
    {
        int pos = bucketPos(key);//找到位置
        const Node* cur = buckets_[pos];
        while (cur)
        {
            if (key == getKey_(cur->obj))
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    Object& findOrInsert(const Object& obj)
    {
        return insertUnique(obj).first->obj;
    }

    Object& findOrInsert(Object&& obj)
    {
        return insertUnique(std::move(obj)).first->obj;
    }


    bool remove(const KeyType& key)
    {
        int pos = bucketPos(key);//找到位置
        Node* cur = buckets_[pos];
        Node* prev = nullptr;
        while (cur)
        {
            if (key == getKey_(cur->obj))
            {
                if (prev == nullptr)
                    buckets_[pos] = cur->next;
                else
                    prev->next = cur->next;
                delete cur;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    void clear()
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
        nodeCnt_ = 0;
    }

    int nodeCnt() const { return nodeCnt_; }

    int bucketCnt() const { return buckets_.size(); }

private:

    template <class X>
    std::pair<Node*, bool> _insertUnique(X&& obj)
    {
        resize(nodeCnt_ + 1);//检查是否需要重建表格

        int pos = bucketPos(getKey_(obj));//找到位置
        Node* cur = buckets_[pos];
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
                return {cur, false};
            cur = cur->next;
        }
        Node* newNode = new Node(std::forward<X>(obj));
        newNode->next = buckets_[pos];
        buckets_[pos] = newNode;
        ++nodeCnt_;
        return {newNode, true};
    }

    template <class X>
    Node* _insertEqual(X&& obj)
    {
        resize(nodeCnt_ + 1);//检查是否需要重建表格

        Node* newNode = new Node(std::forward<X>(obj));
        int pos = bucketPos(getKey_(obj));//找到位置
        Node* cur = buckets_[pos];
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
            {
                newNode->next = cur->next;
                cur->next = newNode;
                ++nodeCnt_;
                return newNode;
            }
            cur = cur->next;
        }
        newNode->next = buckets_[pos];
        buckets_[pos] = newNode;
        ++nodeCnt_;
        return newNode;
    }

    void initBuckets(int size)
    {
        int newSize = roundup(size);
        buckets_.resize(newSize, nullptr);
        nodeCnt_ = 0;
    }

    int bucketPos(const KeyType& key) const
    {
        return bucketPos(key, buckets_.size());
    }

    int bucketPos(const KeyType& key, int n) const
    {
        return hash_(key) % n;
    }

    int roundup(int n)
    {
        return roundupPowerOfTwo(n);
    }

    unsigned int roundupPowerOfTwo(unsigned int i)
    {
        --i;
        i |= i >> 1;
        i |= i >> 2;
        i |= i >> 4;
        i |= i >> 8;
        i |= i >> 16;
        return i + 1;
    }

    void resize(int hintCnt)
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
                int newPos = bucketPos(getKey_(first->obj), newSize);
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

    struct Node
    {
        Node* next;
        Object obj;

        Node(const Object& _obj): obj(_obj) {}
        Node(Object&& _obj): obj(std::move(_obj)) {}
    };

    std::vector<Node*> buckets_;
    int nodeCnt_;

    HashFunc hash_;
    ExtractKey getKey_;
};


#endif //HASH_TABLE_HPP
