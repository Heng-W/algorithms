// 二叉树
#include <assert.h>
#include <vector>
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


    Node* setroot(const T& x) { return _setroot(x); }
    Node* setroot(T&& x) { return _setroot(std::move(x)); }

    Node* addleft(Node* node, const T& x) { return _addleft(node, x); }
    Node* addleft(Node* node, T&& x) { return _addleft(node, std::move(x)); }

    Node* addright(Node* node, const T& x) { return _addright(node, x); }
    Node* addright(Node* node, T&& x) { return _addright(node, std::move(x)); }


    Node* find(const T& data) { return _find(root_, data); }


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
        if (root_ == nullptr) return {};
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
        if (root_ == nullptr) return {};
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
    std::vector<T> levelOrder() const
    {
        if (root_ == nullptr) return {};
        std::vector<T> res;
        std::queue<Node*> nodes;
        nodes.push(root_);
        while (!nodes.empty())
        {
            Node* cur = nodes.front();
            nodes.pop();
            res.push_back(cur->data);
            if (cur->left) nodes.push(cur->left);
            if (cur->right) nodes.push(cur->right);
        }
        return res;
    }

    std::vector<T> morrisInorder() const
    {
        std::vector<T> res;
        Node* cur = root_;
        while (cur)
        {
            if (cur->left == nullptr)
            {
                res.push_back(cur->data);
                cur = cur->right; //将右孩子作为当前节点
            }
            else
            {
                Node* node = cur->left;
                while (node->right != nullptr && cur != node->right)
                    node = node->right;
                if (node->right == nullptr) //还没有线索化，则建立线索
                {
                    node->right = cur;
                    cur = cur->left;
                }
                else //已经线索化，则访问节点并删除线索
                {
                    res.push_back(cur->data);
                    node->right = nullptr;
                    cur = cur->right;
                }
            }
        }
        return res;
    }

    //求深度
    int depth() const { return _depth(root_); }

    void clear() { deleteTree(root_); }

    //中序遍历方式删除
    void clear2()
    {
        if (root_ == nullptr) return;
        std::queue<Node*> nodes;
        nodes.push(root_);
        while (!nodes.empty())
        {
            Node* cur = nodes.front();
            nodes.pop();
            if (cur->left) nodes.push(cur->left);
            if (cur->right) nodes.push(cur->right);
            delete cur;
        }
        root_ = nullptr;
    }

private:
    struct Node
    {
        T data;
        Node* left;
        Node* right;
        Node* parent;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    int _depth(Node* node) const
    {
        return node ? std::max(_depth(node->left), _depth(node->right)) + 1 : 0;
    }


    //设置根结点
    template <class X>
    Node* _setroot(X&& x)
    {
        clear();
        root_ = new Node(std::forward<X>(x));
        root_->left = root_->right = nullptr;
        root_->parent = nullptr;
        return root_;
    }

    //添加左孩子结点
    template <class X>
    Node* _addleft(Node* node, X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->left = newNode->right = nullptr;
        newNode->parent = node;
        node->left = newNode;
        return newNode;
    }

    //添加右孩子结点
    template <class X>
    Node* _addright(Node* node, X&& x)
    {
        Node* newNode = new Node(std::forward<X>(x));
        newNode->left = newNode->right = nullptr;
        newNode->parent = node;
        node->right = newNode;
        return newNode;
    }

    Node* _find(Node* node, const T& data)
    {
        if (node == nullptr)
            return nullptr;
        if (node->data == data)
            return node;
        Node* result = _find(node->left, data);
        if (result)
            return result;
        else
            return _find(node->right, data);
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

    //递归删除子树节点
    void deleteTree(Node*& node)
    {
        if (node)
        {
            deleteTree(node->left);
            deleteTree(node->right);
            delete node;
            node = nullptr;
        }
    }

    Node* root_;
};





template <class T>
void printInfo(const BinaryTree<T>& tree)
{
    using namespace std;
    cout << "depth: " << tree.depth() << endl;
    cout << "morris traversal: ";
    auto res = tree.morrisInorder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
    cout << "preorder traversal: ";
    res = tree.preorder();
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
    cout << "levelOrder traversal: ";
    res = tree.levelOrder();
    for (const auto& x : res) cout << x << " ";
    cout << endl;
}


#include <cstdlib>
#include <ctime>

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
    auto level = tree1.levelOrder();

    BinaryTree<int> tree2(pre.data(), in.data(), in.size(), 0);
    BinaryTree<int> tree3(in.data(), post.data(), in.size(), 1);
    BinaryTree<int> tree4(level.data(), in.data(), in.size(), 2);

    return 0;
}
