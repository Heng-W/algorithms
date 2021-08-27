// 二叉树
#include <assert.h>
#include <stack>
#include <queue>
#include <algorithm>
#include <iostream>


template <class T>
class BinaryTree
{
    class Node;
public:
    BinaryTree(): root_(nullptr) {}

    BinaryTree(const T* input1, const T* input2, int n, int type)
    {
        switch (type)
        {
            case 0:
                root_ = createByPreAndIn(input1, input2, n);
                break;
            case 1:
                root_ = createByInAndPost(input1, input2, n);
                break;
            case 2:
                root_ = createByLevelAndIn(input1, input2, n);
                break;
        }
    }

    ~BinaryTree() { clear(); }


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

    Node* createByInAndPost(const T* inorder, const T* postorder, int n)
    {
        if (n == 0)
            return nullptr;
        Node* node = new Node(postorder[n - 1]);
        int mid = std::find(inorder, inorder + n, node->data) - inorder;
        assert(mid != n);
        node->left = createByInAndPost(inorder, postorder, mid);
        node->right = createByInAndPost(inorder + mid + 1, postorder + mid, n - mid - 1);
        return node;
    }

    Node* createByLevelAndIn(const T* levelorder, const T* inorder, int n)
    {
        std::vector<T> level(n);
        std::copy_n(levelorder, n, level.begin());
        return createByLevelAndIn(level, inorder, n);
    }

    Node* createByLevelAndIn(std::vector<T>& levelorder, const T* inorder, int n)
    {
        if (n == 0)
            return nullptr;
        Node* node = new Node(levelorder[0]);
        int mid = std::find(inorder, inorder + n, node->data) - inorder;
        assert(mid != n);
        // 划分左右子树
        std::vector<T> left;
        std::vector<T> right;
        for (int i = 1; i < n; ++i)
        {
            bool inLeft = false;
            for (int j = 0; j < mid; ++j)
            {
                if (levelorder[i] == inorder[j])
                {
                    inLeft = true;
                    break;
                }
            }
            if (inLeft)
                left.push_back(levelorder[i]);
            else
                right.push_back(levelorder[i]);
        }
        std::vector<T>().swap(levelorder);
        node->left = createByLevelAndIn(left, inorder, mid);
        node->right = createByLevelAndIn(right, inorder + mid + 1, n - mid - 1);
        return node;
    }


    //设置根结点
    template <class X>
    Node* setroot(X&& x)
    {
        clear();
        root_ = new Node(std::forward<X>(x));
        root_->left = root_->right = nullptr;
        root_->parent = nullptr;
        return root_;
    }

    //添加左孩子结点
    template <class X>
    Node* addleft(Node* node, X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->left = newNode->right = nullptr;
        newNode->parent = node;
        node->left = newNode;
        return newNode;
    }

    //添加右孩子结点
    template <class X>
    Node* addright(Node* node, X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->left = newNode->right = nullptr;
        newNode->parent = node;
        node->right = newNode;
        return newNode;
    }


    Node* find(const T& data) { return find(root_, data); }

    Node* find(Node* node, const T& data)
    {
        if (node == nullptr)
            return nullptr;
        if (node->data == data)
            return node;
        Node* result = find(node->left, data);
        if (result)
            return result;
        else
            return find(node->right, data);
    }

    void mirror(Node* node)
    {
        if (node)
        {
            std::swap(node->left, node->right);
            mirror(node->left);
            mirror(node->right);
        }
    }

    std::vector<T> preorder() const
    {
        std::vector<T> res;
        preorder(root_, res);
        return res;
    }
    std::vector<T> inorder() const
    {
        std::vector<T> res;
        inorder(root_, res);
        return res;
    }
    std::vector<T> postorder() const
    {
        std::vector<T> res;
        postorder(root_, res);
        return res;
    }

    //先序遍历（非递归1）
    std::vector<T> preorderI1() const
    {
        std::vector<T> res;
        std::stack<Node*> sta;
        Node* cur = root_;
        while (cur || !sta.empty())
        {
            if (cur)
            {
                res.push_back(cur->data);
                sta.push(cur);
                cur = cur->left;
            }
            else
            {
                cur = sta.top();
                sta.pop();
                cur = cur->right;
            }
        }
        return res;
    }

    //先序遍历（非递归2）
    std::vector<T> preorderI2() const
    {
        std::vector<T> res;
        std::stack<Node*> sta;
        sta.push(root_);
        while (!sta.empty())
        {
            Node* cur = sta.top();
            sta.pop();
            res.push_back(cur->data);
            if (cur->right) sta.push(cur->right);
            if (cur->left) sta.push(cur->left);
        }
        return res;
    }

    //中序遍历（非递归）
    std::vector<T> inorderI(Node* t) const
    {
        std::vector<T> res;
        std::stack<Node*> sta;
        Node* cur = root_;
        while (cur || !sta.empty())
        {
            if (cur)
            {
                sta.push(cur);
                cur = cur->left;
            }
            else
            {
                cur = sta.top();
                sta.pop();
                res.push_back(cur->data);
                cur = cur->right;
            }
        }
        return res;
    }

    //后序遍历（非递归1）
    std::vector<T> postorderI1() const
    {
        std::vector<T> res;
        std::stack<Node*> sta;
        Node* cur = root_;
        Node* prev = nullptr;
        while (cur || !sta.empty())
        {
            while (cur)
            {
                sta.push(cur);
                cur = cur->left;
            }
            cur = sta.top();
            if (!cur->right || prev == cur->right)
            {
                res.push_back(cur->data);
                prev = cur;
                sta.pop();
                cur = nullptr;
            }
            else
            {
                cur = cur->right;
            }
        }
        return res;
    }

    //后序遍历（非递归2）
    std::vector<T> postorderI2() const
    {
        std::vector<T> res;
        std::stack<Node*> sta;
        Node* cur = root_;
        Node* prev = nullptr;
        sta.push(cur);
        while (!sta.empty())
        {
            cur = sta.top();
            if ((cur->left == nullptr && cur->right == nullptr) ||
                    (prev != nullptr && (prev == cur->left || prev == cur->right)))
            {
                res.push_back(cur->data);
                prev = cur;
                sta.pop();
            }
            else
            {
                if (cur->right) sta.push(cur->right);
                if (cur->left) sta.push(cur->left);
            }
        }
        return res;
    }

    //层序遍历
    std::vector<T> leverorder() const
    {
        std::vector<T> res;
        std::queue<Node*> queue;
        queue.push(root_);
        while (!queue.empty())
        {
            Node* cur = queue.front();
            queue.pop();
            res.push_back(cur->data);
            if (cur->left) queue.push(cur->left);
            if (cur->right) queue.push(cur->right);
        }
        return res;
    }

    //求深度
    int depth() const { return _depth(root_); }

    void clear() { deleteTree(root_); }

private:
    struct Node
    {
        T data;
        Node* left;
        Node* right;
        Node* parent;

        template <class X>
        Node(X&& _data): data(std::forward<X>(_data)) {}
    };

    int _depth(Node* node) const
    {
        return node ? std::max(_depth(node->left), _depth(node->right)) + 1 : 0;
    }

    //前序遍历（递归）
    void preorder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            res.push_back(node->data);
            preorder(node->left, res);
            preorder(node->right, res);
        }
    }

    //中序遍历（递归）
    void inorder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            inorder(node->left, res);
            res.push_back(node->data);
            inorder(node->right, res);
        }
    }

    //后序遍历（递归）
    void postorder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            postorder(node->left, res);
            postorder(node->right, res);
            res.push_back(node->data);
        }
    }

    Node*& parentPtr(Node* node)
    {
        Node* parent = node->parent;
        if (parent->left == node)
        {
            return parent->left;
        }
        else
        {
            return parent->right;
        }
    }

    void deleteTree(Node*& node)
    {
        _deleteTree(node);
        node = nullptr;
    }

    //递归删除结点
    void _deleteTree(Node* node)
    {
        if (node)
        {
            _deleteTree(node->left);
            _deleteTree(node->right);
            delete node;
        }
    }

    Node* root_;
};





template <class T>
void printInfo(const BinaryTree<T>& tree)
{
    using namespace std;
    cout << "depth: " << tree.depth() << endl;
    cout << "preorder traversal: ";
    auto res = tree.preorder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
    cout << "inorder traversal: ";
    res = tree.inorder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
    cout << "postorder traversal: ";
    res = tree.postorder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
    cout << "levelorder traversal: ";
    res = tree.leverorder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
}


#include <stdlib.h>
#include <time.h>

int main()
{
    using namespace std;
    srand(time(nullptr));

    BinaryTree<int> tree1;
    auto root = tree1.setroot(4);
    auto cur = tree1.addleft(root, 7);
    tree1.addright(cur, 2);
    tree1.addleft(cur, 20);
    cur = tree1.addright(root, 1);
    tree1.addleft(tree1.find(20), 10);
    tree1.addleft(tree1.find(2), 5);
    printInfo(tree1);

    auto pre = tree1.preorder();
    auto in = tree1.inorder();
    auto post = tree1.postorder();
    auto level = tree1.leverorder();

    BinaryTree<int> tree2(pre.data(), in.data(), in.size(), 0);
    BinaryTree<int> tree3(in.data(), post.data(), in.size(), 1);
    BinaryTree<int> tree4(level.data(), in.data(), in.size(), 2);

    return 0;
}
