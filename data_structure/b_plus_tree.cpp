
#include <functional>

// B+树
// Key：键类型，Value：值类型，M：阶数
template <class Key, class Value, int M>
class BPlusTree
{
    template <class NodePtr> struct Iterator_;
    struct LeafNode;
public:
    using KeyType = Key;
    using ValueType = Value;
    using Iterator = Iterator_<LeafNode*>;
    using ConstIterator = Iterator_<const LeafNode*>;

    BPlusTree(): root_(nullptr) {}
    ~BPlusTree() { clear(); }

    // 拷贝构造函数
    BPlusTree(const BPlusTree& rhs)
    {
        LeafNode* prev = nullptr;
        root_ = clone(rhs.root_, nullptr, prev);
    }

    // 移动构造函数
    BPlusTree(BPlusTree&& rhs) noexcept: root_(rhs.root_)
    { rhs.root_ = nullptr; }

    // 拷贝赋值运算符
    BPlusTree& operator=(const BPlusTree& rhs)
    {
        BPlusTree copy = rhs;
        return *this = std::move(copy);
    }

    // 移动赋值运算符
    BPlusTree& operator=(BPlusTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            rhs.root_ = nullptr;
        }
        return *this;
    }

    // 查找
    std::pair<Iterator, bool> find(const KeyType& key) const;

    // 插入
    bool insert(const KeyType& key, const ValueType& value)
    { return _insert(key, value); }

    bool insert(const KeyType& key, ValueType&& value)
    { return _insert(key, std::move(value)); }

    // 删除
    bool remove(const KeyType& key)
    {
        auto res = find(key);
        bool exist = res.second;
        if (!exist) return false;
        erase(res.first);
        return true;
    }
    Iterator erase(Iterator pos);

    void clear() { if (root_) destroy(root_); }

    // 首尾迭代器
    Iterator begin() { return minimum(); }
    ConstIterator begin() const { return minimum(); }

    Iterator end() { return nullptr; }
    ConstIterator end() const { return nullptr; }

private:
    struct NodeBase;
    struct IndexNode;

    template <class X>
    bool _insert(const KeyType& key, X&& value);

    // 合并节点
    void mergeLeafNode(IndexNode* parent, int pos);
    void mergeIndexNode(IndexNode* parent, int pos);

    void destroy(NodeBase* node);
    static NodeBase* clone(NodeBase* node, IndexNode* parent, LeafNode*& prev);

    LeafNode* minimum() const
    {
        if (!root_) return nullptr;
        NodeBase* cur = root_;
        while (!cur->isLeaf()) cur = static_cast<IndexNode*>(cur)->childs[0];
        return static_cast<LeafNode*>(cur);
    }

    // 键数量的最小值和最大值
    static constexpr int kMinKeyNum = (M + 1) / 2 - 1;
    static constexpr int kMaxKeyNum = M - 1;

    // 定义迭代器
    template <class NodePtr>
    struct Iterator_
    {
        NodePtr node;
        int pos;

        using Self = Iterator_;
        using ValueRef = decltype((node->values[0]));
        using ValuePtr = decltype(&node->values[0]);

        Iterator_() {}
        Iterator_(NodePtr _node, int _pos = 0): node(_node), pos(_pos) {}

        bool operator==(const Self& it) const { return node == it.node && pos == it.pos; }
        bool operator!=(const Self& it) const { return !(*this == it); }

        const KeyType& key() const { return node->keys[pos]; }
        ValueRef operator*() const { return node->values[pos]; }
        ValuePtr operator->() const { return &*this; }

        Self& operator++()
        {
            if (++pos >= node->keyCount)
            {
                node = node->next;
                pos = 0;
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

    // 定义节点基类
    struct NodeBase
    {
        KeyType keys[M];
        int keyCount = 0;
        IndexNode* parent = nullptr;
        const bool leaf;

        NodeBase(bool _leaf): leaf(_leaf) {}

        bool isLeaf() const { return leaf; }
    };

    // 索引节点，包含child指针域
    struct IndexNode : NodeBase
    {
        NodeBase* childs[M + 1] = {nullptr};

        IndexNode(): NodeBase(false) {}
    };

    // 叶子节点，包含value及next指针域
    struct LeafNode : NodeBase
    {
        ValueType values[M];
        LeafNode* next;

        LeafNode(): NodeBase(true) {}
    };

    NodeBase* root_; // 根节点
};


template <class Key, class Value, int M>
auto BPlusTree<Key, Value, M>::
find(const KeyType& key) const -> std::pair<Iterator, bool>
{
    NodeBase* cur = root_;
    int pos = 0;
    while (cur)
    {
        pos = 0;
        while (pos < cur->keyCount && cur->keys[pos] <= key)
            ++pos;
        if (!cur->isLeaf())
            cur = static_cast<IndexNode*>(cur)->childs[pos];
        else if (pos > 0 && key == cur->keys[pos - 1])
            return {Iterator(static_cast<LeafNode*>(cur), pos - 1), true};
        else
            break;
    }
    return {Iterator(static_cast<LeafNode*>(cur), pos), false};
}


template <class Key, class Value, int M>
template <class X>
bool BPlusTree<Key, Value, M>::_insert(const KeyType& key, X&& value)
{
    if (root_ == nullptr)
    {
        LeafNode* node = new LeafNode();
        node->next = nullptr;
        root_ = node;
    }
    auto res = find(key);
    bool exist = res.second;
    if (exist) return false;

    {
        LeafNode* cur = res.first.node;
        int pos = res.first.pos;

        for (int i = cur->keyCount; i > pos; --i)
        {
            cur->keys[i] = cur->keys[i - 1];
            cur->values[i] = std::move(cur->values[i - 1]);
        }
        cur->keys[pos] = key;
        cur->values[pos] = std::forward<X>(value);
        ++cur->keyCount;

        if (cur->keyCount <= kMaxKeyNum) return true;

        // 分裂
        LeafNode* brother = new LeafNode();
        int mid = M / 2;
        for (int i = mid; i < cur->keyCount; ++i)
        {
            brother->keys[i - mid] = cur->keys[i];
            brother->values[i - mid] = std::move(cur->values[i]);
            ++brother->keyCount;
        }
        cur->keyCount -= brother->keyCount;
        brother->next = cur->next;
        cur->next = brother;

        IndexNode* parent = cur->parent;

        if (parent == nullptr)
        {
            parent = new IndexNode();
            parent->childs[0] = cur;
            cur->parent = parent;
            root_ = parent;
        }
        pos = parent->keyCount;
        while (pos > 0 && key < parent->keys[pos - 1])
        {
            parent->keys[pos] = parent->keys[pos - 1];
            parent->childs[pos + 1] = parent->childs[pos];
            --pos;
        }
        parent->keys[pos] = brother->keys[0];
        parent->childs[pos + 1] = brother;
        brother->parent = parent;
        ++parent->keyCount;
    }

    IndexNode* cur = res.first.node->parent;

    while (cur->keyCount > kMaxKeyNum)
    {
        IndexNode* brother = new IndexNode();
        int mid = M / 2;
        int pos = 0;
        for (int i = mid + 1; i < cur->keyCount; ++i)
        {
            brother->keys[pos] = cur->keys[i];
            brother->childs[pos] = cur->childs[i];
            brother->childs[pos]->parent = brother;
            ++pos;
            ++brother->keyCount;
        }
        brother->childs[pos] = cur->childs[cur->keyCount];
        brother->childs[pos]->parent = brother;

        IndexNode* parent = cur->parent;
        if (parent == nullptr)
        {
            parent = new IndexNode();
            parent->childs[0] = cur;
            cur->parent = parent;
            root_ = parent;
        }
        pos = parent->keyCount;
        while (pos > 0 && key < parent->keys[pos - 1])
        {
            parent->keys[pos] = parent->keys[pos - 1];
            parent->childs[pos + 1] = parent->childs[pos];
            --pos;
        }
        parent->keys[pos] = cur->keys[mid];
        parent->childs[pos + 1] = brother;
        brother->parent = parent;
        ++parent->keyCount;

        cur->keyCount -= brother->keyCount + 1;
        cur = cur->parent;
    }
    return true;
}


template <class Key, class Value, int M>
auto BPlusTree<Key, Value, M>::erase(Iterator position) -> Iterator
{
    Iterator next;
    {
        LeafNode* cur = position.node;
        int pos = position.pos;

        // 删除位置后的数据前移
        for (int i = pos; i < cur->keyCount - 1; ++i)
        {
            cur->keys[i] = cur->keys[i + 1];
            cur->values[i] = std::move(cur->values[i + 1]);
        }
        --cur->keyCount;
        if (cur->keyCount >= kMinKeyNum) // 键数量足够，结束
            return pos < cur->keyCount ? position : cur->next;
        // 判断是否是根节点
        if (cur == root_)
        {
            if (cur->keyCount == 0)
            {
                delete root_;
                root_ = nullptr;
            }
            return Iterator(static_cast<LeafNode*>(root_), pos);
        }

        IndexNode* parent = cur->parent;
        int childPos = 0; // 当前节点在parent的位置
        while (cur != parent->childs[childPos]) ++childPos;

        // 如果左兄弟的键数量足够，向其借一个
        if (childPos > 0 && parent->childs[childPos - 1]->keyCount > kMinKeyNum)
        {
            LeafNode* left = static_cast<LeafNode*>(parent->childs[childPos - 1]);
            for (int i = cur->keyCount; i > 0; --i)
            {
                cur->keys[i] = cur->keys[i - 1];
                cur->values[i] = std::move(cur->values[i - 1]);
            }
            cur->keys[0] = left->keys[left->keyCount - 1];;
            cur->values[0] = std::move(left->values[left->keyCount - 1]);
            cur->parent->keys[childPos - 1] = cur->keys[0];
            ++cur->keyCount;
            --left->keyCount;
            return pos + 1 < cur->keyCount ? Iterator(cur, pos + 1) : cur->next;
        }
        // 如果右兄弟的键数量足够，向其借一个
        else if (childPos < parent->keyCount &&
                 parent->childs[childPos + 1]->keyCount > kMinKeyNum)
        {
            LeafNode* right = static_cast<LeafNode*>(parent->childs[childPos + 1]);

            cur->keys[cur->keyCount] = right->keys[0];
            cur->values[cur->keyCount] = std::move(right->values[0]);
            cur->parent->keys[childPos] = right->keys[1];

            ++cur->keyCount;

            for (int i = 0; i < right->keyCount - 1; ++i)
            {
                right->keys[i] = right->keys[i + 1];
                right->values[i] = std::move(right->values[i + 1]);
            }
            --right->keyCount;
            return Iterator(cur, pos);
        }
        else // 兄弟节点的键数量不足，合并节点
        {
            if (childPos > 0)
            {
                LeafNode* left = static_cast<LeafNode*>(parent->childs[childPos - 1]);
                next = pos < cur->keyCount ? Iterator(left, pos + left->keyCount)
                       : cur->next;
                mergeLeafNode(parent, childPos - 1); // 合并到左子树
            }
            else
            {
                next = Iterator(cur, pos);
                mergeLeafNode(parent, childPos); // 右子树合并到当前
            }
        }
    }

    IndexNode* cur = position.node->parent;

    while (cur->keyCount < kMinKeyNum)
    {
        if (cur == root_)
        {
            if (cur->keyCount == 0)
            {
                if (cur->childs[0])
                {
                    root_ = cur->childs[0];
                    root_->parent = nullptr;
                }
                else
                {
                    root_ = nullptr;
                }
                delete cur;
            }
            return next;
        }
        IndexNode* parent = cur->parent;
        int childPos = 0;
        while (cur != parent->childs[childPos]) ++childPos;
        if (childPos > 0 && parent->childs[childPos - 1]->keyCount > kMinKeyNum)
        {
            IndexNode* left = static_cast<IndexNode*>(parent->childs[childPos - 1]);
            for (int i = cur->keyCount; i > 0; --i)
            {
                cur->keys[i] = cur->keys[i - 1];
                cur->childs[i + 1] = cur->childs[i];
            }
            cur->childs[1] = cur->childs[0];
            cur->childs[0] = left->childs[left->keyCount];
            cur->childs[0]->parent = cur;

            cur->keys[0] = parent->keys[childPos - 1];
            cur->parent->keys[childPos - 1] = left->keys[left->keyCount - 1];
            ++cur->keyCount;
            --left->keyCount;
            return next;
        }
        else if (childPos < parent->keyCount &&
                 parent->childs[childPos + 1]->keyCount > kMinKeyNum)
        {
            IndexNode* right = static_cast<IndexNode*>(parent->childs[childPos + 1]);

            cur->keys[cur->keyCount] = parent->keys[childPos];
            cur->parent->keys[childPos] = right->keys[0];

            cur->childs[cur->keyCount + 1] = right->childs[0];
            cur->childs[cur->keyCount + 1]->parent = cur;
            ++cur->keyCount;

            for (int i = 0; i < right->keyCount - 1; ++i)
            {
                right->keys[i] = right->keys[i + 1];
                right->childs[i] = right->childs[i + 1];
            }
            right->childs[right->keyCount - 1] = right->childs[right->keyCount];
            --right->keyCount;
            return next;
        }
        else
        {
            if (childPos > 0)
                mergeIndexNode(parent, childPos - 1); // 合并到左子树
            else
                mergeIndexNode(parent, childPos); // 右子树合并到当前
            cur = parent;
        }
    }
    return next;
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::mergeLeafNode(IndexNode* parent, int pos)
{
    LeafNode* left = static_cast<LeafNode*>(parent->childs[pos]);
    LeafNode* right = static_cast<LeafNode*>(parent->childs[pos + 1]);

    for (int i = pos; i < parent->keyCount - 1; ++i)
    {
        parent->keys[i] = parent->keys[i + 1];
        parent->childs[i + 1] = parent->childs[i + 2];
    }
    --parent->keyCount;

    int leftPos = left->keyCount;
    for (int i = 0; i < right->keyCount; ++i)
    {
        left->keys[leftPos] = right->keys[i];
        left->values[leftPos] = std::move(right->values[i]);
        ++leftPos;
    }
    left->keyCount += right->keyCount;
    left->next = right->next;
    delete right;
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::mergeIndexNode(IndexNode* parent, int pos)
{
    IndexNode* left = static_cast<IndexNode*>(parent->childs[pos]);
    IndexNode* right = static_cast<IndexNode*>(parent->childs[pos + 1]);

    left->keys[left->keyCount] = parent->keys[pos];
    for (int i = pos; i < parent->keyCount - 1; ++i)
    {
        parent->keys[i] = parent->keys[i + 1];
        parent->childs[i + 1] = parent->childs[i + 2];
    }
    --parent->keyCount;

    int leftPos = left->keyCount + 1;
    for (int i = 0; i < right->keyCount; ++i)
    {
        left->keys[leftPos] = right->keys[i];
        left->childs[leftPos] = right->childs[i];
        left->childs[leftPos]->parent = left;
        ++leftPos;
    }
    left->childs[leftPos] = right->childs[right->keyCount];
    left->childs[leftPos]->parent = left;
    left->keyCount += right->keyCount + 1;
    delete right;
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::destroy(NodeBase* cur)
{
    if (cur->isLeaf())
    {
        delete static_cast<LeafNode*>(cur);
        return;
    }
    IndexNode* node = static_cast<IndexNode*>(cur);
    int pos = 0;
    while (pos <= node->keyCount)
    {
        destroy(node->childs[pos++]);
    }
    delete node;
}


template <class Key, class Value, int M>
auto BPlusTree<Key, Value, M>::
clone(NodeBase* node, IndexNode* parent, LeafNode*& prev) -> NodeBase*
{
    if (node == nullptr) return nullptr;

    auto initNode = [&](NodeBase * copy)
    {
        copy->keyCount = node->keyCount;
        copy->parent = parent;
        for (int i = 0; i < node->keyCount; ++i)
        {
            copy->keys[i] = node->keys[i];
        }
    };
    if (!node->isLeaf())
    {
        IndexNode* copy = new IndexNode();
        initNode(copy);
        for (int i = 0; i <= node->keyCount; ++i)
        {
            copy->childs[i] = clone(static_cast<IndexNode*>(node)->childs[i], copy, prev);
        }
        return copy;
    }
    else
    {
        LeafNode* copy = new LeafNode();
        initNode(copy);
        for (int i = 0; i < node->keyCount; ++i)
        {
            copy->values[i] = static_cast<LeafNode*>(node)->values[i];
        }
        copy->next = nullptr;
        if (prev) prev->next = copy;
        prev = copy;
        return copy;
    }
}


// 测试
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iomanip>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 15; ++i)
    {
        vec.push_back(rand() % 100);
    }

    BPlusTree<int, int, 4> tree;
    for (int i = 0; i < (int)vec.size(); ++i)
    {
        tree.insert(vec[i], i);
    }
    for (auto it = tree.begin(); it != tree.end(); ++it)
    {
        cout << "(" << it.key() << "," << *it << ") ";
    }
    cout << endl;

    cout << *tree.find(vec[0]).first << endl;

    auto tree2 = tree;
    for (const auto& x : tree) cout << x << " ";
    cout << endl;

    for (const auto& x : vec) tree.remove(x);

    // 删除value小于5的数据
    for (auto it = tree2.begin(); it != tree2.end();)
    {
        if (*it < 5)
            it = tree2.erase(it);
        else
            ++it;
    }
    for (const auto& x : tree2) cout << x << " ";
    cout << endl;

    return 0;
}