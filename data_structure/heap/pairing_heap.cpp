
#include <assert.h>
#include <functional>
#include <queue>


template <class T, class Compare = std::less<T>>
class PairingHeap
{
    struct Node;
public:
    PairingHeap(): root_(nullptr) {}
    ~PairingHeap() { clear(); }

    Node* insert(const T& data)
    {   
        Node* newNode = new Node(data);
        root_ = merge(root_, newNode); 
        return newNode;
    }

    Node* insert(T&& data)
    { 
        Node* newNode = new Node(std::move(data));
        root_ = merge(root_, newNode); 
        return newNode;
    }

    void merge(PairingHeap& rhs)
    {
        if (this != &rhs)
        {
            root_ = merge(root_, rhs.root_);
            rhs.root_ = nullptr;
        }
    }

    bool setValue(Node* node, const T& newValue)
    {
        if (comp(node->data, newValue))
            return false;
        node->data = newValue;
        if (node == root_) return true;

        if (node == node->prev->child)
            node->prev->child = node->sibling;
        else
            node->prev->sibling = node->sibling;
        if (node->sibling != nullptr)
            node->sibling->prev = node->prev;
        node->sibling = node->prev = nullptr;
        root_ = merge(root_, node);
        return true;
    }

    void pop()
    {
        assert(!empty());
        std::queue<Node*> que;
        for (Node* cur = root_->child; cur; cur = cur->sibling)
        {
            que.push(cur);
        }
        while (que.size() > 1)
        {
            Node* node1 = que.front();
            que.pop();
            Node* node2 = que.front();
            que.pop();
            que.push(merge(node1, node2));
        }
        delete root_;
        if (que.empty())
        {
            root_ = nullptr;
            return;
        }
        root_ = que.front();
        root_->sibling = root_->prev = nullptr;
    }

    void clear() { destroy(root_); }

    const T& top() const { assert(!empty()); return root_->data; }
    bool empty() const { return root_ == nullptr; }

private:

    Node* merge(Node* root1, Node* root2)
    {
        if (root1 == nullptr) return root2;
        if (root2 == nullptr) return root1;

        if (comp(root2->data, root1->data))
        {
            std::swap(root1, root2);
        }
        root2->sibling = root1->child;
        if (root2->sibling)
            root2->sibling->prev = root2;
        root1->child = root2;
        root2->prev = root1;
        return root1;
    }

    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            destroy(node->child);
            destroy(node->sibling);
            delete node;
            node = nullptr;
        }
    }

    static bool comp(const T& lhs, const T& rhs)
    {
        return Compare()(lhs, rhs);
    }

    struct Node
    {
        T data;
        Node* child = nullptr; //最左侧节点
        Node* sibling = nullptr; //右兄弟
        Node* prev = nullptr;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };
    Node* root_;
};



//测试
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>

int main()
{
    using namespace std;
    srand(time(nullptr));

    vector<int> vec1, vec2;
    for (int i = 0; i < 10; ++i)
    {
        vec1.push_back(rand() % 100);
        vec2.push_back(rand() % 100);
    }
    copy(vec1.cbegin(), vec1.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;
    copy(vec2.cbegin(), vec2.cend(), ostream_iterator<int>(cout, " "));
    cout << endl;

    PairingHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.insert(x);

    PairingHeap<int, greater<int>> heap2;
    for (const auto& x : vec2) heap2.insert(x);

    heap1.merge(heap2);

    while (!heap1.empty())
    {
        cout << heap1.top() << " ";
        heap1.pop();
    }
    cout << endl;
    return 0;
}


