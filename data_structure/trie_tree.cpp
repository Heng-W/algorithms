
#include <string>

//字典树
class TrieTree
{
public:
    TrieTree() { root_ = new Node(); }
    ~TrieTree() { clear(); ::free(root_); }

    // 拷贝构造函数
    TrieTree(const TrieTree& rhs) { root_ = clone(rhs.root_); }

    //移动构造函数
    TrieTree(TrieTree&& rhs): TrieTree()
    { std::swap(root_, rhs.root_); }

    //拷贝赋值运算符
    TrieTree& operator=(const TrieTree& rhs)
    {
        TrieTree copy = rhs;
        std::swap(root_, copy.root_);
        return *this;
    }

    //移动赋值运算符
    TrieTree& operator=(TrieTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            std::swap(root_, rhs.root_);
        }
        return *this;
    }

    // 插入单词
    void insert(const std::string& word)
    {
        Node* cur = root_;
        for (int i = 0; i < word.size(); ++i)
        {
            int pos = word[i] - 'a';
            if (cur->childs[pos] == nullptr)
            {
                cur->childs[pos] = new Node();
            }
            cur = cur->childs[pos];
            ++cur->prefixCount;
        }
        ++cur->wordCount;
    }

    // 删除单词
    void remove(const std::string& word)
    {
        int count = findCount(word);
        if (count == 0) return;
        Node* cur = root_;
        for (int i = 0; i < word.size(); ++i)
        {
            int pos = word[i] - 'a';
            cur->childs[pos]->prefixCount -= count;
            if (cur->childs[pos]->prefixCount == 0)
            {
                destroyTree(cur->childs[pos]);
                cur->childs[pos] = nullptr;
                return;
            }
            cur = cur->childs[pos];
        }
        cur->wordCount = 0;
    }

    // 删除某前缀的所有单词
    void removePrefix(const std::string& prefix)
    {
        int count = findPrefix(prefix);
        if (count == 0) return;
        Node* cur = root_;
        for (int i = 0; i < prefix.size(); ++i)
        {
            int pos = prefix[i] - 'a';
            cur->childs[pos]->prefixCount -= count;
            if (cur->childs[pos]->prefixCount == 0)
            {
                destroyTree(cur->childs[pos]);
                cur->childs[pos] = nullptr;
                return;
            }
            cur = cur->childs[pos];
        }
    }

    // 查找单词出现的次数
    int findCount(const std::string& word) const
    {
        Node* cur = root_;
        for (int i = 0; i < word.size(); ++i)
        {
            int pos = word[i] - 'a';
            if (cur->childs[pos] == nullptr) return 0;
            cur = cur->childs[pos];
        }
        return cur->wordCount;
    }

    int findPrefix(const std::string& prefix) const
    {
        Node* cur = root_;
        for (int i = 0; i < prefix.size(); ++i)
        {
            int pos = prefix[i] - 'a';
            if (cur->childs[pos] == nullptr) return 0;
            cur = cur->childs[pos];
        }
        return cur->prefixCount;
    }

    void clear() { destroyTree(root_); }

private:
    static constexpr int CHILD_NUM = 26;

    struct Node;

    void destroyTree(Node* node)
    {
        for (int i = 0; i < CHILD_NUM; ++i)
        {
            if (node->childs[i])
            {
                destroyTree(node->childs[i]);
                delete node->childs[i];
            }
        }
    }

    Node* clone(Node* node)
    {
        Node* copy = new Node();
        copy->wordCount = node->wordCount;
        copy->prefixCount = node->prefixCount;
        for (int i = 0; i < CHILD_NUM; ++i)
        {
            if (node->childs[i] != nullptr)
            {
                copy->childs[i] = clone(node->childs[i]);
            }
        }
        return copy;
    }

    struct Node
    {
        int wordCount = 0; // 单词出现的次数
        int prefixCount = 0; // 此前缀的所有单词数量
        Node* childs[CHILD_NUM] = {nullptr};
    };

    Node* root_; // 根节点
};


// 测试
#include <iostream>

int main()
{
    using namespace std;
    TrieTree tree;
    tree.insert("abcg");
    tree.insert("hjkl");
    tree.insert("abkl");
    tree.insert("abcg");

    cout << tree.findPrefix("ab") << endl;
    cout << tree.findCount("abcg") << endl;

    tree.remove("abcg");
    cout << tree.findCount("abcg") << endl;

    tree.removePrefix("ab");
    cout << tree.findPrefix("ab") << endl;

    return 0;
}
