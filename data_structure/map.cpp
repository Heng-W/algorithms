
#include "rb_tree.hpp"


template <class Key, class Value, class Compare = std::less<Key>>
class Map
{
public:
    template <class Pair>
    struct select1st
    {
        const typename Pair::first_type& operator()(const Pair& pair) const
        { return pair.first; }
    };

    using Object = std::pair<const Key, Value>;
    using Tree = RBTree<Object, Key, select1st<Object>, Compare>;
    using Iterator = typename Tree::Iterator;
    using ConstIterator = typename Tree::ConstIterator;
    using KeyType = typename Tree::KeyType;


    std::pair<Iterator, bool> insert(const Object& obj)
    { return tree_.insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return tree_.insert(std::move(obj)); }

    Iterator find(const KeyType& key) {return tree_.find(key);}
    ConstIterator find(const KeyType& key) const {return tree_.find(key);}

    Value& operator[](const KeyType& key)
    {
        return tree_.findOrInsert({key, Value()}).second;
    }

    bool remove(const KeyType& key) { return tree_.remove(key); }

    void clear() { tree_.clear(); }

    int count() const { return tree_.count(); }

    ConstIterator begin() const { return tree_.begin(); }
    Iterator begin() { return tree_.begin(); }

    ConstIterator end() const { return tree_.end(); }
    Iterator end() { return tree_.end(); }

private:
    Tree tree_;
};


#include <iostream>
#include <memory>


int main()
{
    using namespace std;
    Map<int, unique_ptr<int>> map;
    map.insert({298, make_unique<int>(153)});
    map.insert({320, make_unique<int>(178)});
    map.insert({120, make_unique<int>(134)});
    map.insert({356, make_unique<int>(232)});

    cout << map.count() << endl;

    cout << (map.find(120) != map.end()) << endl;
    cout << (map.find(10) != map.end()) << endl;

    map.remove(298);

    map[110] = make_unique<int>(282);
    map[90] = make_unique<int>(265);

    for (const auto& x : map) cout << *x.second << " ";
    cout << endl;

    return 0;
}

