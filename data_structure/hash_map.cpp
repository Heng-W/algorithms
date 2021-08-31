
#include "hash_table.hpp"


template <class Key, class Value, class HashFunc = std::hash<Key>>
class HashMap
{
    friend std::ostream& operator<<(std::ostream& out, const HashMap& map)
    {
        out << map.table_;
        return out;
    }
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
    using MHashTable = HashTable<Object, HashFunc, select1st<Object>>;
    using Iterator = typename MHashTable::Iterator;
    using ConstIterator = typename MHashTable::ConstIterator;
    using KeyType = typename MHashTable::KeyType;


    HashMap(int n = 32): table_(n) {}

    std::pair<Iterator, bool> insert(const Object& obj)
    { return table_.insertUnique(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return table_.insertUnique(std::move(obj)); }

    Iterator find(const KeyType& key) {return table_.find(key);}
    ConstIterator find(const KeyType& key) const {return table_.find(key);}

    Value& operator[](const KeyType& key)
    {
        return table_.findOrInsert({key, Value()}).second;
    }


    bool remove(const KeyType& key) { return table_.remove(key); }

    void clear() { table_.clear(); }

    int nodeCnt() const { return table_.nodeCnt(); }

private:
    MHashTable table_;
};


int main()
{
    using namespace std;
    HashMap<int, int> map;
    map.insert({298, 153});
    map.insert({190, 123});
    map.insert({892, 132});
    map.insert({92, 456});
    map.insert({122, 125});

    cout << map.nodeCnt() << endl;

    cout << map.find(298) << endl;
    cout << map.find(10) << endl;

    cout << map[20] << endl;
    cout << map[122] << endl;
    cout << map.nodeCnt() << endl;

    map.remove(92);

    return 0;
}

