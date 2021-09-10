
#include <functional>
#include <queue>
#include <iostream>

//B树
template <class Key, class Value, int M>
class BTree
{
    struct Node;
public:
    using Object = std::pair<Key, Value>;
    using KeyType = Key;
    using ValueType = Value;
    using Iterator = Node*;

    BTree(): root_(nullptr) {}

    ~BTree() { clear(); }

    std::pair<Node*, int> find(const KeyType& key) const;

    bool insert(const Object& obj) { return _insert(obj); }
    bool insert(Object&& obj) { return _insert(std::move(obj)); }

    bool remove(const KeyType& key);

    void levelOrder() const;

    void inorder() const { _inorder(root_); }

    void clear() { deleteTree(root_); }

private:

    template <class X>
    bool _insert(X&& obj);

    void mergeNode(Node* parent, int pos);

    void _inorder(Node* node) const;

    void deleteTree(Node* node);


    static const KeyType& getKey(const Object& obj)
    { return obj.first; }

    static constexpr int KEY_MIN = (M + 1) / 2 - 1;
    static constexpr int KEY_MAX = M - 1;

    struct Node
    {
        Object objects[M];
        Node* childs[M + 1] = {nullptr};
        Node* parent = nullptr;
        int keyCount = 0;
    };
    Node* root_;
};


template <class Key, class Value, int M>
auto BTree<Key, Value, M>::find(const KeyType& key) const -> std::pair<Node*, int>
{
    Node* cur = root_;
    while (cur)
    {
        int pos = 0;
        while (pos < cur->keyCount)
        {
            if (key < getKey(cur->objects[pos]))
                break;
            else if (getKey(cur->objects[pos]) < key)
                ++pos;
            else
                return {cur, pos};
        }
        cur = cur->childs[pos];
    }
    return {nullptr, -1};
}


template <class Key, class Value, int M>
template <class X>
bool BTree<Key, Value, M>::_insert(X&& obj)
{
    if (root_ == nullptr)
    {
        root_ = new Node();
        root_->objects[0] = std::forward<X>(obj);
        root_->keyCount = 1;
        return true;
    }
    const KeyType& key = getKey(obj);
    Node* cur = root_;
    Node* parent = nullptr;
    int pos;
    while (cur)
    {
        pos = 0;
        while (pos < cur->keyCount)
        {
            if (key < getKey(cur->objects[pos]))
                break;
            else if (getKey(cur->objects[pos]) < key)
                ++pos;
            else
                return false;
        }
        parent = cur;
        cur = cur->childs[pos];
    }
    cur = parent;
    Node* sub = nullptr;
    Object tmp = std::forward<X>(obj);
    while (true)
    {
        for (int i = cur->keyCount; i > pos; --i)
        {
            cur->objects[i] = std::move(cur->objects[i - 1]);
            cur->childs[i + 1] = cur->childs[i];
        }
        cur->objects[pos] = std::move(tmp);
        cur->childs[pos + 1] = sub;
        if (sub)
        {
            sub->parent = cur;
        }
        if (++cur->keyCount < M)
        {
            return true;
        }
        Node* brother = new Node();
        int mid = M / 2;
        pos = 0;
        for (int i = mid + 1; i < cur->keyCount; ++i)
        {
            brother->objects[pos] = std::move(cur->objects[i]);
            brother->childs[pos] = cur->childs[i];
            if (brother->childs[pos])
                brother->childs[pos]->parent = brother;
            ++pos;
            ++brother->keyCount;
        }
        brother->childs[pos] = cur->childs[cur->keyCount];
        if (brother->childs[pos])
            brother->childs[pos]->parent = brother;
        cur->keyCount -= (brother->keyCount + 1);
        if (cur->parent)
        {
            tmp = std::move(cur->objects[mid]);
            sub = brother;
            cur = cur->parent;
            pos = cur->keyCount;
            while (pos > 0 && key < getKey(cur->objects[pos - 1])) --pos;
        }
        else
        {
            root_ = new Node();
            root_->objects[0] = std::move(cur->objects[mid]);
            root_->childs[0] = cur;
            root_->childs[1] = brother;
            root_->keyCount = 1;
            cur->parent = root_;
            brother->parent = root_;
            return true;
        }
    }
}


template <class Key, class Value, int M>
bool BTree<Key, Value, M>::remove(const KeyType& key)
{
    auto res = find(key);
    Node* cur = res.first;
    int pos = res.second;
    if (cur == nullptr)
        return false;
    if (cur->childs[pos + 1])
    {
        Node* sub = cur->childs[pos + 1];
        while (sub->childs[0])
            sub = sub->childs[0];
        //后继节点替换
        cur->objects[pos] = std::move(sub->objects[0]);
        cur = sub;
        pos = 0;
    }
    //删除节点
    for (int i = pos; i < cur->keyCount - 1; ++i)
    {
        cur->objects[i] = std::move(cur->objects[i + 1]);
    }
    --cur->keyCount;
    while (cur->keyCount < KEY_MIN)
    {
        if (cur == root_)
        {
            if (cur->keyCount == 0)
            {
                if (root_->childs[0])
                {
                    root_ = root_->childs[0];
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
        Node* parent = cur->parent;
        int childPos = 0;
        while (cur != parent->childs[childPos]) ++childPos;
        if (childPos > 0 && parent->childs[childPos - 1]->keyCount > KEY_MIN)
        {
            Node* left = parent->childs[childPos - 1];
            for (int i = cur->keyCount; i > 0; --i)
            {
                cur->objects[i] = std::move(cur->objects[i - 1]);
                cur->childs[i + 1] = cur->childs[i];
            }
            cur->childs[1] = cur->childs[0];
            cur->childs[0] = left->childs[left->keyCount];
            if (cur->childs[0])
                cur->childs[0]->parent = cur;

            cur->objects[0] = std::move(parent->objects[childPos - 1]);
            cur->parent->objects[childPos - 1] = std::move(left->objects[left->keyCount - 1]);
            ++cur->keyCount;
            --left->keyCount;
            return true;
        }
        else if (childPos < parent->keyCount && parent->childs[childPos + 1]->keyCount > KEY_MIN)
        {
            Node* right = parent->childs[childPos + 1];

            cur->objects[cur->keyCount] = std::move(parent->objects[childPos]);
            cur->parent->objects[childPos] = std::move(right->objects[0]);

            cur->childs[cur->keyCount + 1] = right->childs[0];
            if (cur->childs[cur->keyCount + 1])
                cur->childs[cur->keyCount + 1]->parent = cur;
            ++cur->keyCount;

            for (int i = 0; i < right->keyCount - 1; ++i)
            {
                right->objects[i] = std::move(right->objects[i + 1]);
                right->childs[i] = right->childs[i + 1];
            }
            right->childs[right->keyCount - 1] = right->childs[right->keyCount];
            --right->keyCount;
            return true;
        }
        else
        {
            if (childPos > 0)
                mergeNode(parent, childPos - 1); //合并到左子树
            else
                mergeNode(parent, childPos); //右子树合并到当前
            cur = parent;
        }
    }
    return true;
}


template <class Key, class Value, int M>
void BTree<Key, Value, M>::mergeNode(Node* parent, int pos)
{
    Node* left = parent->childs[pos];
    Node* right = parent->childs[pos + 1];

    left->objects[left->keyCount] = std::move(parent->objects[pos]);
    for (int i = pos; i < parent->keyCount - 1; ++i)
    {
        parent->objects[i] = std::move(parent->objects[i + 1]);
        parent->childs[i + 1] = parent->childs[i + 2];
    }
    --parent->keyCount;

    int leftPos = left->keyCount + 1;
    for (int i = 0; i < right->keyCount; ++i)
    {
        left->objects[leftPos] = std::move(right->objects[i]);
        left->childs[leftPos] = right->childs[i];
        if (left->childs[leftPos])
            left->childs[leftPos]->parent = left;
        ++leftPos;
    }
    left->childs[leftPos] = right->childs[right->keyCount];
    if (left->childs[leftPos])
        left->childs[leftPos]->parent = left;
    left->keyCount += right->keyCount + 1;
    delete right;
}


template <class Key, class Value, int M>
void BTree<Key, Value, M>::levelOrder() const
{
    if (root_ == nullptr) return;
    Node* cur = root_;
    std::queue<Node*> nodes;
    nodes.push(cur);
    while (!nodes.empty())
    {
        Node* cur = nodes.front();
        nodes.pop();
        for (int i = 0; i < cur->keyCount; ++i)
            std::cout << getKey(cur->objects[i]) << " ";
        for (int i = 0; i <= cur->keyCount; ++i)
        {
            if (cur->childs[i])
                nodes.push(cur->childs[i]);
        }
    }
}


template <class Key, class Value, int M>
void BTree<Key, Value, M>::_inorder(Node* node) const
{
    if (node == nullptr)
        return;
    int pos = 0;
    while (pos < node->keyCount)
    {
        _inorder(node->childs[pos]);
        std::cout << getKey(node->objects[pos]) << " ";
        ++pos;
    }
    _inorder(node->childs[pos]);
}


template <class Key, class Value, int M>
void BTree<Key, Value, M>::deleteTree(Node* node)
{
    if (node == nullptr)
        return;
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
    for (int i = 0; i < 15; ++i)
    {
        vec.push_back(rand() % 100);
    }

    BTree<int, int, 4> tree;
    for (int i = 0; i < 15; ++i)
    {
        tree.insert({vec[i], i});
    }

    tree.inorder();
    cout << endl;
    tree.levelOrder();
    cout << endl;

    tree.find(vec[0]);

    for (int i = 0; i < 15; ++i)
    {
        tree.remove(vec[i]);
        tree.inorder();
        cout << endl;
    }

    return 0;
}
