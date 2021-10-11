
#include <memory>

static constexpr int PRIME_NUMS[] =
{
    2, 3, 5, 7, 11, 13, 17, 19, 23, 29,
    31, 37, 41, 43, 47, 53
};

//哈希树
template <class Key, class Value>
class HashTree
{
    struct Node;
public:
    using KeyType = Key;
    using ValueType = Value;
    using Object = std::pair<KeyType, ValueType>;
    using Iterator = Node*;
    using ConstIterator = const Node*;


    HashTree() { root_ = createNode(PRIME_NUMS[0]); }

    ~HashTree() { clear(); ::free(root_); }
    
    //拷贝构造函数
    HashTree(const HashTree& rhs) { root_ = clone(rhs.root_); }

    //移动构造函数
    HashTree(HashTree&& rhs): HashTree()
    { std::swap(root_, rhs.root_); }

    //拷贝赋值运算符
    HashTree& operator=(const HashTree& rhs)
    {
        HashTree copy = rhs;
        std::swap(root_, copy.root_);
        return *this;
    }

    //移动赋值运算符
    HashTree& operator=(HashTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            std::swap(root_, rhs.root_);
        }
        return *this;
    }


    const Node* find(KeyType key) const
    { return _find(root_, 0, key); }

    Node* find(KeyType key)
    { return const_cast<Node*>(_find(root_, 0, key)); }

    std::pair<Node*, bool> insert(KeyType key, const ValueType& value)
    { return _insert(root_, 0, key, value); }

    std::pair<Node*, bool> insert(KeyType key, ValueType&& value)
    { return _insert(root_, 0, key, std::move(value)); }


    bool remove(KeyType key) { return _remove(root_, 0, key); }

    void clear() { destroyTree(root_, 0); }

private:

    const Node* _find(const Node* node, int level, KeyType key) const
    {
        if (node->occupied && key == node->key)
        {
            return node;
        }
        int index = key % PRIME_NUMS[level];
        if (node->childs[index] == nullptr)
        {
            return nullptr;
        }
        return _find(node->childs[index], level + 1, key);
    }

    template <class X>
    std::pair<Node*, bool> _insert(Node* node, int level, KeyType key, X&& value)
    {
        if (!node->occupied)
        {
            node->key = key;
            construct(&node->value, std::forward<X>(value));
            node->occupied = true;
            return {node, true};
        }
        else if (key == node->key)
        {
            return {node, false};
        }
        int index = key % PRIME_NUMS[level];
        if (node->childs[index] == nullptr)
        {
            node->childs[index] = createNode(PRIME_NUMS[level + 1]);
        }
        return _insert(node->childs[index], level + 1, key, std::forward<X>(value));
    }

    bool _remove(Node* node, int level, int key)
    {
        if (node->occupied && key == node->key)
        {
            destroy(&node->value);
            node->occupied = false;
            return true;
        }
        int index = key % PRIME_NUMS[level];
        if (node->childs[index] == nullptr)
        {
            return false;
        }
        return _remove(node->childs[index], level + 1, key);
    }

    // 销毁node子树
    void destroyTree(Node* node, int level)
    {
        if (node->occupied)
            destroy(&node->value);
        for (int i = 0; i < PRIME_NUMS[level]; ++i)
        {
            if (node->childs[i] != nullptr)
            {
                destroyTree(node->childs[i], level + 1);
                ::free(node->childs[i]);
            }
        }
    }

    Node* clone(Node* node, int level)
    {
        Node* copy = createNode(PRIME_NUMS[level]);
        copy->key = node->key;
        if (node->occupied)
        {
            construct(&copy->value, node->value);
            copy->occupied = true;
        }
        for (int i = 0; i < PRIME_NUMS[level]; ++i)
        {
            if (node->childs[i] != nullptr)
            {
                copy->childs[i] = clone(node->childs[i], level + 1);
            }
        }
        return copy;
    }

    static Node* createNode(int childNum)
    { return (Node*)::calloc(1, sizeof(Node) + sizeof(Node*) * (childNum - 1)); }

    template <class X>
    static void construct(ValueType* ptr, X&& x)
    { new (ptr) ValueType(std::forward<X>(x)); }

    static void destroy(ValueType* ptr)
    { ptr->~ValueType(); }


    struct Node
    {
        KeyType key;
        ValueType value;
        bool occupied; //节点是否被占据
        Node* childs[1];
    };

    Node* root_;

};


#include <string>
#include <iostream>

int main()
{
    using namespace std;
    HashTree<int, string> tree;
    tree.insert(30,"hello");
    tree.insert(10,"test");

    auto res = tree.find(30);
    if (res) cout << res->value << endl;

    cout << tree.remove(30) <<endl;
    res = tree.find(30);
    if (res) cout << res->value << endl;

    return 0;
}

