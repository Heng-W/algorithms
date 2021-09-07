
#include "linked_hash_table.hpp"


template <class Key, class Value, class HashFunc = std::hash<Key>>
class LinkedHashMap
{
public:
    template <class Pair>
    struct select1st
    {
        const typename Pair::first_type& operator()(const Pair& pair) const
        {
            return pair.first;
        }
    };

    using Object = std::pair<const Key, Value>;
    using Container = LinkedHashTable<Object, HashFunc, select1st<Object>>;
    using Iterator = typename Container::Iterator;
    using ConstIterator = typename Container::ConstIterator;
    using KeyType = typename Container::KeyType;


    LinkedHashMap(int n = 32): table_(n) {}

    std::pair<Iterator, bool> insert(const Object& obj)
    { return table_.insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return table_.insert(std::move(obj)); }

    Iterator find(const KeyType& key) {return table_.find(key);}
    ConstIterator find(const KeyType& key) const {return table_.find(key);}

    Value& operator[](const KeyType& key)
    {
        return table_.findOrInsert({key, Value()}).second;
    }


    Iterator erase(Iterator it) { return table_.erase(it); }
    
    void removeFirst() { return table_.removeFirst();) }

    bool remove(const KeyType& key) { return table_.remove(key); }


    void clear() { table_.clear(); }

    int nodeCount() const { return table_.nodeCount(); }

    ConstIterator begin() const { return table_.begin(); }
    Iterator begin() { return table_.begin(); }

    ConstIterator end() const { return table_.end(); }
    Iterator end() { return table_.end(); }

private:
    Container table_;
};


#include <iostream>

int main()
{
    using namespace std;
    LinkedHashMap<int, int> map;
    map.insert({298, 153});
    map.insert({190, 123});
    map.insert({892, 132});
    map.insert({92, 456});
    map.insert({122, 125});

    cout << map.nodeCount() << endl;

    cout << (map.find(298) != map.end()) << endl;
    cout << (map.find(10) != map.end()) << endl;

    cout << map[20] << endl;
    cout << map[122] << endl;

    map.remove(92);
    map.erase(map.find(190));

    for (const auto& x : map) cout << x.second << " ";
    cout << endl;

    return 0;
}
