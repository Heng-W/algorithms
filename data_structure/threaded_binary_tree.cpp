
#include <assert.h>
#include <algorithm>
#include <iostream>

template <class T>
class ThreadedBinaryTree
{
    struct Node;
public:
    ThreadedBinaryTree(const T* input1, const T* input2, int n)
    {
        root_ = createByPreAndIn(input1, input2, n);
        Node* prev = nullptr;
        inThreading(root_, prev);
    }

    ~ThreadedBinaryTree() { clear(); }

    Node* createByPreAndIn(const T* preorder, const T* inorder, int n)
    {
        if (n == 0)
            return nullptr;
        Node* node = new Node(preorder[0]);
        int mid = std::find(inorder, inorder + n, node->data) - inorder;
        assert(mid != n);
        node->left = createByPreAndIn(preorder + 1, inorder, mid);
        node->right = createByPreAndIn(preorder + mid + 1, inorder + mid + 1, n - mid - 1);
        return node;
    }

    void preThreading(Node* node, Node*& prev)
    {
        if (node == nullptr) return;
        if (node->left == nullptr)
        {
            node->leftTag = THREAD;
            node->left = prev;
        }
        if (prev && prev->right == nullptr)
        {
            prev->rightTag = THREAD;
            prev->right = node;
        }
        prev = node;
        if (node->leftTag == LINK)
        {
            preThreading(node->left, prev);
        }
        preThreading(node->right, prev);
    }

    void preOrder() const
    {
        Node* cur = root_;
        while (cur)
        {
            while (cur->leftTag == LINK)
            {
                std::cout << cur->data << " ";
                cur = cur->left;
            }
            std::cout << cur->data << " ";
            cur = cur->right;
        }
    }

    void inThreading(Node* node, Node*& prev)
    {
        if (node == nullptr) return;
        inThreading(node->left, prev);
        if (node->left == nullptr)
        {
            node->leftTag = THREAD;
            node->left = prev;
        }
        if (prev && prev->right == nullptr)
        {
            prev->rightTag = THREAD;
            prev->right = node;
        }
        prev = node;
        inThreading(node->right, prev);
    }

    void inOrder() const
    {
        Node* cur = root_;
        while (cur)
        {
            while (cur->leftTag == LINK)
                cur = cur->left;
            std::cout << cur->data << " ";
            while (cur->rightTag == THREAD)
            {
                cur = cur->right;
                std::cout << cur->data << " ";
            }
            cur = cur->right;
        }
    }

    void clear()
    {
        Node* cur = root_;
        while (cur)
        {
            while (cur->leftTag == LINK)
                cur = cur->left;
            Node* tmp = cur;
            while (cur->rightTag == THREAD)
            {
                cur = cur->right;
                delete tmp;
                tmp = cur;
            }
            cur = cur->right;
            delete tmp;
        }
    }


private:
    enum PtrTag : char {LINK, THREAD};

    struct Node
    {
        T data;
        Node* left;
        Node* right;
        PtrTag leftTag;
        PtrTag rightTag;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* root_;

};

int main()
{
    using namespace std;
    int pre[] = {3, 1, 0, 2, 4};
    int in[] = {0, 1, 2, 3, 4};
    ThreadedBinaryTree<int> tree(pre, in, 5);
    tree.inOrder();
    cout << endl;
    return 0;
}

