
#include "hash_table.hpp"

// 基于哈希表的无序map
template <class Key, class Value, class HashFunc = std::hash<Key>>
class HashMap
{
public:
    template <class Pair>
    struct select1st
    {
        const typename Pair::first_type& operator()(const Pair& pair) const
        { return pair.first; }
    };

    using Object = std::pair<const Key, Value>;
    using MHashTable = HashTable<Object, HashFunc, select1st<Object>>;
    using Iterator = typename MHashTable::Iterator;
    using ConstIterator = typename MHashTable::ConstIterator;
    using KeyType = typename MHashTable::KeyType;


    HashMap(int n = 32): table_(n) {}

    // 插入
    std::pair<Iterator, bool> insert(const Object& obj)
    { return table_.insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return table_.insert(std::move(obj)); }

    // 查找
    Iterator find(const KeyType& key) { return table_.find(key);}
    ConstIterator find(const KeyType& key) const { return table_.find(key);}

    Value& operator[](const KeyType& key)
    { return table_.findOrInsert({key, Value()}).second; }

    // 删除   
    bool remove(const KeyType& key) { return table_.remove(key); }

    // 清除
    void clear() { table_.clear(); }

    // 元素数量
    int size() const { return table_.size(); }

    // 首尾迭代器
    ConstIterator begin() const { return table_.begin(); }
    Iterator begin() { return table_.begin(); }

    ConstIterator end() const { return table_.end(); }
    Iterator end() { return table_.end(); }

private:
    MHashTable table_; // 底层容器：哈希表
};


// 测试
#include <iostream>

int main()
{
    using namespace std;
    HashMap<int, int> map;
    map.insert({298, 153});
    map.insert({190, 123});
    map.insert({892, 132});
    map.insert({92, 456});
    map.insert({122, 125});

    cout << map.size() << endl;

    cout << (map.find(298) != map.end()) << endl;
    cout << (map.find(10) != map.end()) << endl;

    cout << map[20] << endl;
    cout << map[122] << endl;

    map.remove(92);

    for (const auto& x : map) cout << x.second << " ";
    cout << endl;

    return 0;
}

