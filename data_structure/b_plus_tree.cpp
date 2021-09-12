
#include <functional>
#include <queue>
#include <iostream>

//B+树
template <class Key, class Value, int M>
class BPlusTree
{
    struct NodeBase;
    struct IndexNode;
    struct LeafNode;
public:
    using KeyType = Key;
    using ValueType = Value;
    using Iterator = LeafNode*;

    BPlusTree(): root_(nullptr) {}

    ~BPlusTree() { clear(); }

    std::tuple<LeafNode*, int, bool> find(const KeyType& key) const;

    bool insert(const KeyType& key, const ValueType& value)
    { return _insert(key, value); }

    bool insert(const KeyType& key, ValueType&& value)
    { return _insert(key, std::move(value)); }

    bool remove(const KeyType& key);

    void levelOrder() const;

    void traversal() const;

    void clear() { if (root_) deleteTree(root_); }

private:

    template <class X>
    bool _insert(const KeyType& key, X&& value);

    void mergeLeafNode(IndexNode* parent, int pos);
    void mergeIndexNode(IndexNode* parent, int pos);


    void deleteTree(NodeBase* node);


    LeafNode* minimum() const
    {
        if (!root_) return nullptr;
        NodeBase* cur = root_;
        while (!cur->isLeaf()) cur = ((IndexNode*)cur)->childs[0];
        return (LeafNode*)cur;
    }


    static constexpr int KEY_MIN = (M + 1) / 2 - 1;
    static constexpr int KEY_MAX = M - 1;


    struct NodeBase
    {
        KeyType keys[M];
        int keyCount = 0;
        IndexNode* parent = nullptr;
        const bool leaf;

        NodeBase(bool _leaf): leaf(_leaf) {}

        bool isLeaf() const { return leaf; }
    };

    struct IndexNode : NodeBase
    {
        NodeBase* childs[M + 1] = {nullptr};

        IndexNode(): NodeBase(false) {}
    };

    struct LeafNode : NodeBase
    {
        ValueType values[M];
        LeafNode* next;

        LeafNode(): NodeBase(true) {}
    };
    NodeBase* root_;
};


template <class Key, class Value, int M>
auto BPlusTree<Key, Value, M>::find(const KeyType& key) const -> std::tuple<LeafNode*, int, bool>
{
    NodeBase* cur = root_;
    int pos = 0;
    while (cur)
    {
        pos = 0;
        while (pos < cur->keyCount && cur->keys[pos] <= key)
            ++pos;
        if (!cur->isLeaf())
            cur = ((IndexNode*)cur)->childs[pos];
        else if (pos > 0 && key == cur->keys[pos - 1])
            return {(LeafNode*)cur, pos - 1, true};
        else
            break;
    }
    return {(LeafNode*)cur, pos, false};
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
    bool exist = std::get<2>(res);
    if (exist) return false;

    {
        LeafNode* cur = std::get<0>(res);
        int pos = std::get<1>(res);

        for (int i = cur->keyCount; i > pos; --i)
        {
            cur->keys[i] = cur->keys[i - 1];
            cur->values[i] = std::move(cur->values[i - 1]);
        }
        cur->keys[pos] = key;
        cur->values[pos] = std::forward<X>(value);
        ++cur->keyCount;

        if (cur->keyCount <= KEY_MAX) return true;

        //分裂
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

    IndexNode* cur = std::get<0>(res)->parent;

    while (cur->keyCount > KEY_MAX)
    {
        IndexNode* brother = new IndexNode();
        int mid = M / 2;
        for (int i = mid + 1; i < cur->keyCount; ++i)
        {
            brother->keys[i - mid - 1] = cur->keys[i];
            brother->childs[i - mid - 1] = cur->childs[i];
            brother->childs[i - mid - 1]->parent = brother;
            ++brother->keyCount;
        }
        brother->childs[brother->keyCount] = cur->childs[cur->keyCount];
        brother->childs[brother->keyCount]->parent = brother;

        cur->keyCount -= (brother->keyCount + 1);

        IndexNode* parent = cur->parent;

        if (parent == nullptr)
        {
            parent = new IndexNode();
            parent->childs[0] = cur;
            cur->parent = parent;
            root_ = parent;
        }
        int pos = parent->keyCount;
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

        cur = cur->parent;
    }
    return true;
}


template <class Key, class Value, int M>
bool BPlusTree<Key, Value, M>::remove(const KeyType& key)
{
    auto res = find(key);
    bool exist = std::get<2>(res);
    if (!exist) return false;

    {
        LeafNode* cur = std::get<0>(res);
        int pos = std::get<1>(res);

        //删除节点
        for (int i = pos; i < cur->keyCount - 1; ++i)
        {
            cur->keys[i] = cur->keys[i + 1];
            cur->values[i] = std::move(cur->values[i + 1]);
        }
        --cur->keyCount;

        if (cur->keyCount >= KEY_MIN) return true;

        if (cur == root_)
        {
            if (cur->keyCount == 0)
            {
                delete root_;
                root_ = nullptr;
            }
            return true;
        }

        IndexNode* parent = cur->parent;
        int childPos = 0;
        while (cur != parent->childs[childPos]) ++childPos;
        if (childPos > 0 && parent->childs[childPos - 1]->keyCount > KEY_MIN)
        {
            LeafNode* left = (LeafNode*)parent->childs[childPos - 1];
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
            return true;
        }
        else if (childPos < parent->keyCount && parent->childs[childPos + 1]->keyCount > KEY_MIN)
        {
            LeafNode* right = (LeafNode*)parent->childs[childPos + 1];

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
            return true;
        }
        else
        {
            if (childPos > 0)
                mergeLeafNode(parent, childPos - 1); //合并到左子树
            else
                mergeLeafNode(parent, childPos); //右子树合并到当前
        }
    }

    IndexNode* cur = std::get<0>(res)->parent;

    while (cur->keyCount < KEY_MIN)
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
            return true;
        }
        IndexNode* parent = cur->parent;
        int childPos = 0;
        while (cur != parent->childs[childPos]) ++childPos;
        if (childPos > 0 && parent->childs[childPos - 1]->keyCount > KEY_MIN)
        {
            IndexNode* left = (IndexNode*)parent->childs[childPos - 1];
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
            return true;
        }
        else if (childPos < parent->keyCount && parent->childs[childPos + 1]->keyCount > KEY_MIN)
        {
            IndexNode* right = (IndexNode*)parent->childs[childPos + 1];

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
            return true;
        }
        else
        {
            if (childPos > 0)
                mergeIndexNode(parent, childPos - 1); //合并到左子树
            else
                mergeIndexNode(parent, childPos); //右子树合并到当前
            cur = parent;
        }
    }
    return true;
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::mergeLeafNode(IndexNode* parent, int pos)
{
    LeafNode* left = (LeafNode*)parent->childs[pos];
    LeafNode* right = (LeafNode*)parent->childs[pos + 1];

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
    IndexNode* left = (IndexNode*)parent->childs[pos];
    IndexNode* right = (IndexNode*)parent->childs[pos + 1];

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
void BPlusTree<Key, Value, M>::levelOrder() const
{
    if (root_ == nullptr) return;
    NodeBase* cur = root_;
    std::queue<NodeBase*> nodes;
    nodes.push(cur);
    while (!nodes.empty())
    {
        NodeBase* cur = nodes.front();
        nodes.pop();
        for (int i = 0; i < cur->keyCount; ++i)
            std::cout << cur->keys[i] << " ";
        for (int i = 0; i <= cur->keyCount; ++i)
        {
            if (!cur->isLeaf())
                nodes.push(((IndexNode*)cur)->childs[i]);
        }
    }
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::traversal() const
{
    LeafNode* cur = minimum();
    while (cur)
    {
        int pos = 0;
        while (pos < cur->keyCount)
        {
            std::cout << cur->keys[pos] << " ";
            ++pos;
        }
        cur = cur->next;
    }
}


template <class Key, class Value, int M>
void BPlusTree<Key, Value, M>::deleteTree(NodeBase* cur)
{
    if (cur->isLeaf())
    {
        delete (LeafNode*)cur;
        return;
    }
    IndexNode* node = (IndexNode*)cur;
    int pos = 0;
    while (pos <= node->keyCount)
    {
        deleteTree(node->childs[pos++]);
    }
    delete node;
}


#include <cstdlib>
#include <ctime>
#include <vector>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec;
    for (int i = 0; i < 10; ++i)
    {
        vec.push_back(rand() % 100);
    }

    BPlusTree<int, int, 4> tree;
    for (int i = 0; i < vec.size(); ++i)
    {
        tree.insert(vec[i], i);
    }
    cout << endl;

    tree.traversal();
    cout << endl;
    tree.levelOrder();
    cout << endl;

    for (int i = 0; i < vec.size(); ++i)
    {
        auto res = tree.find(vec[i]);
        cout << std::get<2>(res) << " ";
    }

    cout << endl;


    for (int i = 0; i < vec.size(); ++i)
    {
        tree.remove(vec[i]);
        tree.traversal();
        cout << endl;
    }

    return 0;
}
