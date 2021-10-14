
#include <cstdlib>
#include <functional>
#include <vector>
#include <unordered_map> // for copy SkipList


// 跳跃表
template <class Object, class Key = Object,
          class ExtractKey = std::_Identity<Object>,
          class Compare = std::less<Key>>
class SkipList
{
    template <class NodePtr> struct Iterator_;
    struct Node;
public:
    using Iterator = Iterator_<Node*>;
    using ConstIterator = Iterator_<const Node*>;
    using KeyType = Key;

    SkipList(int maxLevel = 8):
        level_(0), maxLevel_(maxLevel), nodeCount_(0)
    {
        head_ = (Node*)::calloc(1, sizeof(Node) + sizeof(Node*) * (maxLevel_));
    }

    ~SkipList()
    {
        clear();
        ::free(head_);
    }

    // 拷贝构造函数
    SkipList(const SkipList& rhs): SkipList(rhs.maxLevel_)
    {
        std::unordered_map<const Node*, Node*> created;
        for (int i = rhs.level_; i >= 0; --i)
        {
            const Node* cur = rhs.head_;
            Node* copy = head_;

            while (cur->forward[i])
            {
                cur = cur->forward[i];
                auto it = created.find(cur);
                if (it == created.end()) // 节点未创建
                {
                    copy->forward[i] = createNode(cur->obj, i);
                    created[cur] = copy->forward[i]; // 插入map，标识已创建
                }
                else // 节点已创建
                {
                    copy->forward[i] = it->second;
                }
                copy = copy->forward[i];
            }
        }
        level_ = rhs.level_;
        nodeCount_ = rhs.nodeCount_;
    }

    //移动构造函数
    SkipList(SkipList&& rhs)
        : SkipList() { swap(rhs); }

    //拷贝赋值运算符
    SkipList& operator=(const SkipList& rhs)
    {
        SkipList copy = rhs;
        swap(copy);
        return *this;
    }

    //移动赋值运算符
    SkipList& operator=(SkipList&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    //交换
    void swap(SkipList& rhs)
    {
        using std::swap;
        swap(head_, rhs.head_);
        swap(level_, rhs.level_);
        swap(maxLevel_, rhs.maxLevel_);
        swap(nodeCount_, rhs.nodeCount_);
    }

    //插入元素
    std::pair<Iterator, bool> insert(const Object& obj)
    { return _insert(obj); }

    std::pair<Iterator, bool> insert(Object&& obj)
    { return _insert(std::move(obj)); }

    //查找
    ConstIterator find(const KeyType& key) const
    { return _find(key); }

    Iterator find(const KeyType& key)
    { return const_cast<Node*>(_find(key)); }

    //删除
    Iterator remove(const KeyType& key)
    {
        std::vector<Node*> update(level_ + 1);

        Node* cur = head_;
        for (int i = level_; i >= 0; --i)
        {
            while (cur->forward[i] && comp(getKey(cur->forward[i]->obj), key))
                cur = cur->forward[i];
            update[i] = cur;
        }
        cur = cur->forward[0];

        if (!cur || comp(key, getKey(cur->obj)))
            return nullptr;

        for (int i = 0; i <= level_; ++i)
        {
            if (cur != update[i]->forward[i])
                break;
            update[i]->forward[i] = cur->forward[i];
        }
        while (level_ > 0 && !head_->forward[level_]) --level_;
        Node* next = cur->forward[0];
        destroyNode(cur);
        --nodeCount_;
        return next;
    }

    void clear()
    {
        Node* cur = head_->forward[0];
        while (cur != nullptr)
        {
            Node* next = cur->forward[0];
            delete cur;
            cur = next;
        }
        std::fill_n(&head_->forward[0], level_ + 1, nullptr);
        level_ = 0;
        nodeCount_ = 0;
    }

    ConstIterator begin() const { return head_->forward[0]; }
    Iterator begin() { return head_->forward[0]; }

    ConstIterator end() const { return nullptr; }
    Iterator end() { return nullptr; }


    int count() const { return nodeCount_; }


private:

    template <class X>
    std::pair<Iterator, bool> _insert(X&& obj)
    {
        std::vector<Node*> update(maxLevel_ + 1);

        Node* cur = head_;
        for (int i = level_; i >= 0; --i)
        {
            while (cur->forward[i] && comp(getKey(cur->forward[i]->obj), getKey(obj)))
                cur = cur->forward[i];
            update[i] = cur;
        }
        cur = cur->forward[0];

        if (cur && !comp(getKey(obj), getKey(cur->obj)))
            return {cur, false};

        int level = randomLevel();
        if (level > level_)
        {
            for (int i = level_ + 1; i <= level; ++i)
                update[i] = head_;
            level_ = level;
        }
        Node* node = createNode(std::forward<Object>(obj), level);
        for (int i = 0; i <= level; ++i)
        {
            node->forward[i] = update[i]->forward[i];
            update[i]->forward[i] = node;
        }
        ++nodeCount_;
        return {node, true};
    }


    const Node* _find(const KeyType& key) const
    {
        const Node* cur = head_;
        for (int i = level_; i >= 0; --i)
        {
            while (cur->forward[i] && comp(getKey(cur->forward[i]->obj), key))
                cur = cur->forward[i];
        }
        cur = cur->forward[0];
        if (comp(key, getKey(cur->obj)))
            return nullptr;
        else
            return cur;
    }


    static const KeyType& getKey(const Object& obj)
    { return ExtractKey()(obj);}


    static bool comp(const KeyType& key1, const KeyType& key2)
    { return Compare()(key1, key2); }


    int randomLevel() const
    {
        int level = 0;
        while (level < maxLevel_ && std::rand() % 2) ++level;
        return level;
    }

    template <class X>
    static Node* createNode(X&& obj, int level = 0)
    {
        Node* node = (Node*)::calloc(1, sizeof(Node) + sizeof(Node*)*level);
        new (&node->obj) Object(std::forward<X>(obj));
        return node;
    }

    static void destroyNode(Node* node)
    {
        node->obj.~Object();
        ::free(node);
    }


    template <class NodePtr>
    struct Iterator_
    {
        using Self = Iterator_;

        NodePtr node;

        using ObjectRef = decltype((node->obj));
        using ObjectPtr = decltype(&node->obj);

        Iterator_() {}
        Iterator_(NodePtr _node): node(_node) {}

        bool operator==(const Self& it) const { return node == it.node; }
        bool operator!=(const Self& it) const { return node != it.node; }

        ObjectRef operator*() const { return node->obj; }
        ObjectPtr operator->() const { return &*this; }

        Self& operator++() { node = node->forward[0]; return *this; }
        Self operator++(int)
        {
            Self tmp = *this;
            ++*this;
            return tmp;
        }
    };


    struct Node
    {
        Object obj;
        Node* forward[1];
    };

    Node* head_;
    int level_;
    int maxLevel_;
    int nodeCount_;
};


// 测试
#include <ctime>
#include <iostream>

int main()
{
    using namespace std;
    srand(time(nullptr));

    SkipList<int> list;
    list.insert(298);
    list.insert(190);
    list.insert(892);
    list.insert(92);
    list.insert(122);

    cout << list.count() << endl;

    cout << (list.find(298) != list.end()) << endl;
    cout << (list.find(10) != list.end()) << endl;

    auto list2 = list;
    for (const auto& x : list2) cout << x << " ";
    cout << endl;

    list.remove(92);

    for (const auto& x : list) cout << x << " ";
    cout << endl;

    return 0;
}



