
#include <functional>
#include <vector>

//二项堆
template <class T, class Compare = std::less<T>>
class BinomialHeap
{
public:
    BinomialHeap(): roots_(1), nodeCount_(0) {}

    BinomialHeap(const T& data): roots_(1), nodeCount_(1)
    { roots_[0] = new Node(data); }

    BinomialHeap(T&& data) : roots_(1), nodeCount_(1)
    { roots_[0] = new Node(std::move(data)); }

    ~BinomialHeap() { clear(); }

    BinomialHeap(const BinomialHeap& rhs)
        : roots_(rhs.roots_.size()), nodeCount_(rhs.nodeCount_)
    {
        for (int i = 0; i < rhs.roots_.size(); ++i)
            roots_[i] = clone(rhs.roots_[i]);
    }

    BinomialHeap(BinomialHeap&& rhs)
        : roots_(std::move(rhs.roots_)), nodeCount_(rhs.nodeCount_) 
    {
        rhs.nodeCount_ = 0;
    }

    BinomialHeap& operator=(const BinomialHeap& rhs)
    {
        BinomialHeap copy = rhs;
        swap(copy);
        return *this;
    }

    BinomialHeap& operator=(BinomialHeap&& rhs)
    {
        if (this != &rhs)
        {
            clear();
            swap(rhs);
        }
        return *this;
    }

    void swap(BinomialHeap& rhs)
    {
        using std::swap;
        roots_.swap(rhs.roots_);
        swap(nodeCount_, rhs.nodeCount_);
    }

    void insert(const T& data) { merge(data); }
    void insert(T&& data) { merge(std::move(data)); }

    void pop()
    {
        int minIndex = findMinIndex();
        Node* oldRoot = roots_[minIndex];
        Node* child = oldRoot->child;
        delete oldRoot;

        BinomialHeap heap;
        heap.roots_.resize(minIndex + 1);
        heap.nodeCount_ = (1 << minIndex) - 1;
        for (int i = minIndex - 1; i >= 0; --i)
        {
            heap.roots_[i] = child;
            child = child->next;
            heap.roots_[i]->next = nullptr;
        }
        roots_[minIndex] = nullptr;
        nodeCount_ -= heap.nodeCount_ + 1;
        merge(heap);
    }


    void merge(BinomialHeap& rhs) { merge(std::move(rhs)); }

    void merge(BinomialHeap&& rhs)
    {
        if (this == &rhs)
            return;

        nodeCount_ += rhs.nodeCount_;

        if (nodeCount_ > capacity())
        {
            int oldNum = roots_.size();
            int newNum = std::max(roots_.size(), rhs.roots_.size()) + 1;
            roots_.resize(newNum);
            for (int i = oldNum; i < newNum; ++i)
                roots_[i] = nullptr;
        }

        Node* carry = nullptr;
        for (int i = 0, j = 1; j <= nodeCount_; ++i, j *= 2)
        {
            Node* root1 = roots_[i];
            Node* root2 = i < rhs.roots_.size() ? rhs.roots_[i] : nullptr;
            int caseNum = root1 == nullptr ? 0 : 1;
            caseNum += root2 == nullptr ? 0 : 2;
            caseNum += carry == nullptr ? 0 : 4;

            switch (caseNum)
            {
                case 0: //no trees
                case 1: //only this
                    break;
                case 2:
                    roots_[i] = root2; //only rhs
                    rhs.roots_[i] = nullptr;
                    break;
                case 4: //only carry
                    roots_[i] = carry;
                    carry = nullptr;
                    break;
                case 3: //this and rhs
                    carry = combineTrees(root1, root2);
                    roots_[i] = rhs.roots_[i] = nullptr;
                    break;
                case 5: //this and carry
                    carry = combineTrees(root1, carry);
                    roots_[i] = nullptr;
                    break;
                case 6: //rhs and carry
                    carry = combineTrees(root2, carry);
                    rhs.roots_[i] = nullptr;
                    break;
                case 7: //all three
                    roots_[i] = carry;
                    carry = combineTrees(root1, root2);
                    rhs.roots_[i] = nullptr;
                    break;
            }
        }
        for (auto& root : rhs.roots_)
            root = nullptr;
        rhs.nodeCount_ = 0;
    }

    void clear()
    {
        for (auto& root : roots_)
            destroy(root);
        nodeCount_ = 0;
    }

    const T& top() const { return roots_[findMinIndex()]->data; }

    bool empty() const { return nodeCount_ == 0; }

private:

    struct Node;

    int findMinIndex() const
    {
        int pos = 0;
        int minIndex;
        while (roots_[pos] == nullptr) ++pos;
        for (minIndex = pos; pos < roots_.size(); ++pos)
            if (roots_[pos] != nullptr &&
                    comp(roots_[pos]->data, roots_[minIndex]->data))
                minIndex = pos;
        return minIndex;
    }

    Node* combineTrees(Node* root1, Node* root2)
    {
        if (comp(root2->data, root1->data))
            return combineTrees(root2, root1);
        root2->next = root1->child;
        root1->child = root2;
        return root1;
    }

    int capacity() const { return (1 << roots_.size()) - 1; }

    void destroy(Node*& node)
    {
        if (node != nullptr)
        {
            destroy(node->child);
            destroy(node->next);
            delete node;
            node = nullptr;
        }
    }

    Node* clone(Node* node) const
    {
        if (node == nullptr)
            return nullptr;
        return new Node(node->data, clone(node->child), clone(node->next));
    }

    static bool comp(const T& lhs, const T& rhs)
    { return Compare()(lhs, rhs); }

    struct Node
    {
        T data;
        Node* child; //第一个子节点
        Node* next; //兄弟

        Node(const T& _data, Node* _child = nullptr, Node* _next = nullptr)
            : data(_data), child(_child), next(_next) {}

        Node(T&& _data, Node* _child = nullptr, Node* _next = nullptr)
            : data(std::move(_data)), child(_child), next(_next) {}
    };

    std::vector<Node*> roots_;
    int nodeCount_;
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

    BinomialHeap<int, greater<int>> heap1;
    for (const auto& x : vec1) heap1.insert(x);

    BinomialHeap<int, greater<int>> heap2;
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
