#ifndef HASH_TABLE_HPP
#define HASH_TABLE_HPP

#include <vector>
#include <iostream>

//哈希表
template <class Object, class ExtractKey, class HashFunc>
class HashTable
{
    friend std::ostream& operator<<(std::ostream& out, const HashTable& ht)
    {
        for (int i = 0; i < ht.buckets_.size(); ++i)
        {
            Node* cur = ht.buckets_[i];
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

    HashTable(int n):
        nodeCnt_(0),
        getKey_(ExtractKey()),
        hash_(HashFunc())
    {
        initBuckets(n);
    }

    template <class X>
    bool insertUnique(X&& obj)
    {
        resize(nodeCnt_ + 1);//检查是否需要重建表格

        int pos = position(obj);//找到位置
        Node* cur = buckets_[pos];
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
                return false;
            cur = cur->next;
        }
        Node* tmp = new Node(std::forward<X>(obj));
        tmp->next = buckets_[pos];
        buckets_[pos] = tmp;
        ++nodeCnt_;
        return true;
    }

    template <class X>
    void insertEqual(X&& obj)
    {
        resize(nodeCnt_ + 1);//检查是否需要重建表格

        int pos = position(obj);//找到位置
        Node* cur = buckets_[pos];
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
            {
                Node* tmp = new Node(std::forward<X>(obj));
                tmp->next = cur->next;
                cur->next = tmp;
                ++nodeCnt_;
                return;
            }
            cur = cur->next;
        }
        Node* tmp = new Node(std::forward<X>(obj));
        tmp->next = buckets_[pos];
        buckets_[pos] = tmp;
        ++nodeCnt_;
    }

    Node* find(const Object& obj)
    {
        const auto* tmp = this;
        return const_cast<Node*>(tmp->find(obj));
    }

    const Node* find(const Object& obj) const
    {
        int pos = position(obj);//找到位置
        const Node* cur = buckets_[pos];
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
                return cur;
            cur = cur->next;
        }
        return nullptr;
    }

    bool remove(const Object& obj)
    {
        int pos = position(obj);//找到位置
        Node* cur = buckets_[pos];
        Node* prev = nullptr;
        while (cur)
        {
            if (getKey_(obj) == getKey_(cur->obj))
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

    int bucketSize() const { return buckets_.size(); }

private:
    void initBuckets(int size)
    {
        int newSize = roundup(size);
        buckets_.resize(newSize, nullptr);
        nodeCnt_ = 0;
    }

    int position(const Object& obj) const
    {
        return position(obj, buckets_.size());
    }

    int position(const Object& obj, int n) const
    {
        return hash_(getKey_(obj)) % n;
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
                int newPos = position(first->obj, newSize);
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

        template <class X>
        Node(X&& _obj): obj(std::forward<X>(_obj)) {}
    };

    std::vector<Node*> buckets_;
    int nodeCnt_;

    ExtractKey getKey_;
    HashFunc hash_;
};

#endif //HASH_TABLE_HPP
