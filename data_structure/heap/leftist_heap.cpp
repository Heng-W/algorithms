
#include <assert.h>
#include <functional>

//左式堆
template <class T, class Compare = std::less<T>>
class LeftistHeap
{
public:
    LeftistHeap(): root_(nullptr) {}
    ~LeftistHeap() { clear(); }

    LeftistHeap(const LeftistHeap& rhs)
    { root_ = clone(rhs.root_); }

    LeftistHeap(LeftistHeap&& rhs): root_(rhs.root_)
    { rhs.root_ = nullptr; }

    LeftistHeap& operator=(const LeftistHeap& rhs)
    {
        LeftistHeap copy = rhs;
        std::swap(root_, copy.root_);
        return *this;
    }

    LeftistHeap& operator=(LeftistHeap&& rhs)
    {
        if (this != &rhs)
        {
            clear();
            std::swap(root_, rhs.root_);
        }
        return *this;
    }
    
    void insert(const T& data)
    { root_ = merge(root_, new Node(data)); }

    void insert(T&& data)
    { root_ = merge(root_, new Node(std::move(data))); }

    void pop()
    {
        assert(!empty());
        Node* old = root_;
        root_ = merge(root_->left, root_->right);
        delete old;
    }

    void merge(LeftistHeap& rhs)
    {
        if (this != &rhs)
        {
            root_ = merge(root_, rhs.root_);
            rhs.root_ = nullptr;
        }
    }

    void clear() { destroy(root_); }

    const T& top() const { assert(!empty()); return root_->data; }

    bool empty() const { return root_ == nullptr; }

private:

    struct Node;

    Node* merge(Node* root1, Node* root2)
    {
        if (root1 == nullptr)
            return root2;
        if (root2 == nullptr)
            return root1;
        if (comp(root2->data, root1->data))
            std::swap(root1, root2);
        if (root1->left == nullptr)
        {
            root1->left = root2;
            return root1;
        }
        root1->right = merge(root1->right, root2);
        if (root1->left->npl < root1->right->npl)
        {
            std::swap(root1->left, root1->right);
        }
        root1->npl = root1->right->npl + 1;
        return root1;
    }


    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    Node* clone(Node* node) const
    {
        if (node == nullptr)
            return nullptr;
        return new Node(node->data, clone(node->left), clone(node->right), node->npl);
    }

    static bool comp(const T& lhs, const T& rhs) {
        return Compare()(lhs, rhs);
    }

    struct Node
    {
        T data;
        Node* left;
        Node* right;
        int npl; //零路径长

        Node(const T& _data, Node* _left = nullptr,
             Node* _right = nullptr, int _npl = 0)
            : data(_data), left(_left), right(_right), npl(_npl) {}

        Node(T&& _data, Node* _left = nullptr,
             Node* _right = nullptr, int _npl = 0)
            : data(std::move(_data)), left(_left), right(_right), npl(_npl) {}
    };

    Node* root_;
};


//测试
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <iterator>

int main(){
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

    LeftistHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.insert(x);

    LeftistHeap<int, greater<int>> heap2;
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

