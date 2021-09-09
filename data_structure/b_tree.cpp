
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


    BTree(): root_(nullptr) {}

    ~BTree() { clear(); }


    std::pair<Node*, int> find(const KeyType& key) const
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

    bool insert(const Object& obj) { return _insert(obj); }
    bool insert(Object&& obj) { return _insert(std::move(obj)); }

    void levelOrder()
    { 
        if(root_==nullptr) return;
        Node* cur=root_;
		std::queue<Node*> nodes;
		nodes.push(cur);
		while(!nodes.empty())
		{
			Node* cur = nodes.front();
			nodes.pop();
			for(int i = 0; i < cur->keyCount; ++i)
				std::cout << getKey(cur->objects[i]) <<" ";
			for(int i = 0; i <= cur->keyCount; ++i){
                if(cur->childs[i])
				    nodes.push(cur->childs[i]);
            }
		}
	}



    void inorder() const { _inorder(root_); }

    void clear() { deleteTree(root_); }

private:

    template <class X>
    bool _insert(X&& obj)
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
            int mid = (M - 1) / 2;
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

    void _inorder(Node* node) const
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

    void deleteTree(Node* node)
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

    static const KeyType& getKey(const Object& obj)
    { return obj.first; }

    struct Node
    {
        Object objects[M];
        Node* childs[M + 1] = {nullptr};
        Node* parent = nullptr;
        int keyCount = 0;
    };

    Node* root_;
};


#include <cstdlib>
#include <ctime>
#include <vector>

int main()
{
    using namespace std;
    srand(time(nullptr));

    BTree<int, int, 4> tree;
    for (int i = 0; i < 15; ++i)
    {
        tree.insert({rand() % 100, i});
    }
    tree.inorder();
    cout << endl;
    tree.levelOrder();
    cout << endl;

    return 0;
}
