
#include <functional>

//左式堆
template <class T, class Compare = std::less<T>>
class SkewHeap
{
public:
    SkewHeap(): root_(nullptr) {}
    ~SkewHeap() { clear(); }

    SkewHeap(const SkewHeap& rhs)
    { root_ = clone(rhs.root_); }

    SkewHeap(SkewHeap&& rhs): root_(rhs.root_)
    { rhs.root_ = nullptr; }

    SkewHeap& operator=(const SkewHeap& rhs)
    {
        SkewHeap copy = rhs;
        std::swap(root_, copy.root_);
        return *this;
    }

    SkewHeap& operator=(SkewHeap&& rhs)
    {
        if (this != &rhs)
        {
            clear();
            std::swap(root_, rhs.root_);
        }
        return *this;
    }

    void insert(const T& data)
    { root_ = merge(new Node(data), root_); }

    void insert(T&& data)
    { root_ = merge(new Node(std::move(data)), root_); }

    void pop()
    {
        Node* old = root_;
        root_ = merge(root_->left, root_->right);
        delete old;
    }

    void merge(SkewHeap& rhs)
    {
        if (this != &rhs)
        {
            root_ = merge(root_, rhs.root_);
            rhs.root_ = nullptr;
        }
    }

    void clear()
    {
        destroy(root_);
        root_ = nullptr;
    }

    const T& top() const { return root_->data; }

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
        //合并后直接交换
        std::swap(root1->left, root1->right);
        return root1;
    }

    void destroy(Node* node)
    {
        if (node != nullptr)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
        }
    }

    Node* clone(Node* node) const
    {
        if (node == nullptr)
            return nullptr;
        return new Node(node->data, clone(node->left), clone(node->right));
    }

    static bool comp(const T& lhs, const T& rhs) {
        return Compare()(lhs, rhs);
    }

    struct Node
    {
        T data;
        Node* left;
        Node* right;

        Node(const T& _data, Node* _left = nullptr, Node* _right = nullptr)
            : data(_data), left(_left), right(_right){}

        Node(T&& _data, Node* _left = nullptr, Node* _right = nullptr)
            : data(std::move(_data)), left(_left), right(_right) {}
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

    SkewHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.insert(x);

    SkewHeap<int, greater<int>> heap2;
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

