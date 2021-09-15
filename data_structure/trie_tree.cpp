
#include <string>

//字典树
class TrieTree
{
public:

    TrieTree() { root_ = new Node(); }
    ~TrieTree() { clear(); }

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
                deleteTree(cur->childs[pos]);
                cur->childs[pos] = nullptr;
                return;
            }
            cur = cur->childs[pos];
        }
        cur->wordCount = 0;
    }

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
                deleteTree(cur->childs[pos]);
                cur->childs[pos] = nullptr;
                return;
            }
            cur = cur->childs[pos];
        }
    }

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

    void clear() { deleteTree(root_); }

private:
    static constexpr int CHILD_NUM = 26;

    struct Node;

    void deleteTree(Node* node)
    {
        for (int i = 0; i < CHILD_NUM; ++i)
        {
            if (node->childs[i])
                deleteTree(node->childs[i]);
        }
        delete node;
    }

    struct Node
    {
        int wordCount = 0;
        int prefixCount = 0;
        Node* childs[CHILD_NUM] = {nullptr};
    };
    Node* root_;
};


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