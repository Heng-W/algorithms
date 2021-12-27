
#include "rb_tree.hpp"

// 基于红黑树的有序集合
template <class Object, class Compare = std::less<Object>>
class Set
{
public:
    using Tree = RBTree<Object, Object, std::_Identity<Object>, Compare>;
    using Iterator = typename Tree::Iterator;
    using ConstIterator = typename Tree::ConstIterator;
    using KeyType = typename Tree::KeyType;

    // 插入
    std::pair<Iterator, bool> insert(const Object& obj)
    { return tree_.insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return tree_.insert(std::move(obj)); }

    // 查找
    Iterator find(const KeyType& key) {return tree_.find(key);}
    ConstIterator find(const KeyType& key) const {return tree_.find(key);}

    // 删除
    bool remove(const KeyType& key) { return tree_.remove(key); }

    void clear() { tree_.clear(); }

    int size() const { return tree_.size(); }

    ConstIterator begin() const { return tree_.begin(); }
    Iterator begin() { return tree_.begin(); }

    ConstIterator end() const { return tree_.end(); }
    Iterator end() { return tree_.end(); }

private:
    Tree tree_; // 底层容器：红黑树
};


// 测试
#include <iostream>

int main()
{
    using namespace std;
    Set<int> set;
    set.insert(298);
    set.insert(190);
    set.insert(892);
    set.insert(92);
    set.insert(122);

    cout << set.size() << endl;

    cout << (set.find(298) != set.end()) << endl;
    cout << (set.find(10) != set.end()) << endl;

    set.remove(92);

    for (const auto& x : set) cout << x << " ";
    cout << endl;

    return 0;
}
