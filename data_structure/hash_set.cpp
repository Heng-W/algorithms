
#include "hash_table.hpp"

template <class Object, class HashFunc = std::hash<Object>>
class HashSet
{
    friend std::ostream& operator<<(std::ostream& out, const HashSet& set)
    {
        out << set.table_;
        return out;
    }
public:
    using MHashTable = HashTable<Object, HashFunc, std::_Identity<Object>>;
    using Iterator = typename MHashTable::Iterator;
    using ConstIterator = typename MHashTable::ConstIterator;
    using KeyType = typename MHashTable::KeyType;

    HashSet(int n = 32): table_(n) {}

    std::pair<Iterator, bool> insert(const Object& obj)
    { return table_.insertUnique(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return table_.insertUnique(std::move(obj)); }

    Iterator find(const KeyType& key) {return table_.find(key);}
    ConstIterator find(const KeyType& key) const {return table_.find(key);}

    bool remove(const KeyType& key) { return table_.remove(key); }

    void clear() { table_.clear(); }

    int nodeCnt() const { return table_.nodeCnt(); }

private:
    MHashTable table_;
};


int main()
{
    using namespace std;
    HashSet<int> set(4);
    set.insert(298);
    set.insert(190);
    set.insert(892);
    set.insert(92);
    set.insert(122);

    cout << set.nodeCnt() << endl;

    cout << set.find(298) << endl;
    cout << set.find(10) << endl;

    cout << set << endl;

    set.remove(92);
    cout << set << endl;

    return 0;
}

