
#include <assert.h>
#include <vector>
#include <stack>
#include <queue>
#include <algorithm>
#include <iostream>

// 二叉树
template <class T>
class BinaryTree
{
    struct Node;
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

    // 拷贝构造函数
    BinaryTree(const BinaryTree& rhs) { root_ = clone(rhs.root_); }

    // 移动构造函数
    BinaryTree(BinaryTree&& rhs) noexcept: root_(rhs.root_)
    { rhs.root_ = nullptr; }

    // 拷贝赋值运算符
    BinaryTree& operator=(const BinaryTree& rhs)
    {
        Node* newRoot = clone(rhs.root_);
        clear();
        root_ = newRoot;
        return *this;
    }

    // 移动赋值运算符
    BinaryTree& operator=(BinaryTree&& rhs) noexcept
    {
        if (this != &rhs)
        {
            clear();
            root_ = rhs.root_;
            rhs.root_ = nullptr;
        }
        return *this;
    }

    Node* createByPreAndIn(const T* preOrder, const T* inOrder, int n)
    {
        if (n == 0) return nullptr;
        Node* node = new Node(preOrder[0]);
        int mid = std::find(inOrder, inOrder + n, node->data) - inOrder;
        assert(mid != n);
        node->left = createByPreAndIn(preOrder + 1, inOrder, mid);
        node->right = createByPreAndIn(preOrder + mid + 1, inOrder + mid + 1, n - mid - 1);
        return node;
    }

    Node* createByInAndPost(const T* inOrder, const T* postOrder, int n)
    {
        if (n == 0) return nullptr;
        Node* node = new Node(postOrder[n - 1]);
        int mid = std::find(inOrder, inOrder + n, node->data) - inOrder;
        assert(mid != n);
        node->left = createByInAndPost(inOrder, postOrder, mid);
        node->right = createByInAndPost(inOrder + mid + 1, postOrder + mid, n - mid - 1);
        return node;
    }

    Node* createByLevelAndIn(const T* levelOrder, const T* inOrder, int n)
    {
        std::vector<T> level(n);
        std::copy_n(levelOrder, n, level.begin());
        return createByLevelAndIn(level, inOrder, n);
    }

    Node* createByLevelAndIn(std::vector<T>& levelOrder, const T* inOrder, int n)
    {
        if (n == 0) return nullptr;
        Node* node = new Node(levelOrder[0]);
        int mid = std::find(inOrder, inOrder + n, node->data) - inOrder;
        assert(mid != n);
        // 划分左右子树
        std::vector<T> left;
        std::vector<T> right;
        for (int i = 1; i < n; ++i)
        {
            bool inLeft = false;
            for (int j = 0; j < mid; ++j)
            {
                if (levelOrder[i] == inOrder[j])
                {
                    inLeft = true;
                    break;
                }
            }
            if (inLeft)
                left.push_back(levelOrder[i]);
            else
                right.push_back(levelOrder[i]);
        }
        std::vector<T>().swap(levelOrder);
        node->left = createByLevelAndIn(left, inOrder, mid);
        node->right = createByLevelAndIn(right, inOrder + mid + 1, n - mid - 1);
        return node;
    }

    // 设置根结点
    Node* setRoot(const T& data)
    {
        clear();
        root_ = new Node(data);
        root_->left = root_->right = nullptr;
        root_->parent = nullptr;
        return root_;
    }

    // 添加左孩子结点
    Node* addLeft(Node* pos, const T& data)
    {
        Node* node = new Node(data);
        node->left = node->right = nullptr;
        node->parent = pos;
        pos->left = node;
        return node;
    }

    // 添加右孩子结点
    Node* addRight(Node* pos, const T& data)
    {
        Node* node = new Node(data);
        node->left = node->right = nullptr;
        node->parent = pos;
        pos->right = node;
        return node;
    }

    // 查找
    const Node* find(const T& data) const
    { return _find(root_, data); }

    Node* find(const T& data)
    { return const_cast<Node*>(_find(root_, data)); }


    void mirror(Node* node)
    {
        if (node)
        {
            std::swap(node->left, node->right);
            mirror(node->left);
            mirror(node->right);
        }
    }

    std::vector<T> preOrder() const
    {
        std::vector<T> res;
        preOrder(root_, res);
        return res;
    }
    std::vector<T> inOrder() const
    {
        std::vector<T> res;
        inOrder(root_, res);
        return res;
    }
    std::vector<T> postOrder() const
    {
        std::vector<T> res;
        postOrder(root_, res);
        return res;
    }

    // 先序遍历（非递归1）
    std::vector<T> preOrderI1() const
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

    // 先序遍历（非递归2）
    std::vector<T> preOrderI2() const
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

    // 中序遍历（非递归）
    std::vector<T> inOrderI(Node* t) const
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

    // 后序遍历（非递归1）
    std::vector<T> postOrderI1() const
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

    // 后序遍历（非递归2）
    std::vector<T> postOrderI2() const
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

    // 层序遍历
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
                cur = cur->right; // 将右孩子作为当前节点
            }
            else
            {
                Node* node = cur->left;
                while (node->right != nullptr && cur != node->right)
                    node = node->right;
                if (node->right == nullptr) // 还没有线索化，则建立线索
                {
                    node->right = cur;
                    cur = cur->left;
                }
                else // 已经线索化，则访问节点并删除线索
                {
                    res.push_back(cur->data);
                    node->right = nullptr;
                    cur = cur->right;
                }
            }
        }
        return res;
    }

    // 求深度
    int depth() const { return _depth(root_); }

    void clear() { destroy(root_); }

    // 层序遍历方式删除
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
    struct Node;

    Node* clone(Node* node)
    {
        if (node == nullptr) return nullptr;
        Node* copy = new Node(node->data);
        copy->left = clone(node->left);
        copy->right = clone(node->right);
        return copy;
    }

    static int _depth(Node* node)
    { return node ? std::max(_depth(node->left), _depth(node->right)) + 1 : 0; }

    Node* _find(Node* node, const T& data)
    {
        if (node == nullptr) return nullptr;
        if (node->data == data) return node;
        Node* result = _find(node->left, data);
        return result != nullptr ? result : _find(node->right, data);
    }

    void preOrder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            res.push_back(node->data);
            preOrder(node->left, res);
            preOrder(node->right, res);
        }
    }

    void inOrder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            inOrder(node->left, res);
            res.push_back(node->data);
            inOrder(node->right, res);
        }
    }

    void postOrder(Node* node, std::vector<T>& res) const
    {
        if (node)
        {
            postOrder(node->left, res);
            postOrder(node->right, res);
            res.push_back(node->data);
        }
    }

    void destroy(Node*& node)
    {
        if (node)
        {
            destroy(node->left);
            destroy(node->right);
            delete node;
            node = nullptr;
        }
    }

    // 定义节点
    struct Node
    {
        T data;
        Node* left;
        Node* right;
        Node* parent;

        Node(const T& _data): data(_data) {}
        Node(T&& _data): data(std::move(_data)) {}
    };

    Node* root_;
};


//测试
#include <cstdlib>
#include <ctime>

int main()
{
    using namespace std;
    srand(time(nullptr));

    BinaryTree<int> tree1;
    auto root = tree1.setRoot(4);
    auto cur = tree1.addLeft(root, 7);
    tree1.addRight(cur, 2);
    tree1.addLeft(cur, 20);
    cur = tree1.addRight(root, 1);
    tree1.addLeft(tree1.find(20), 10);
    tree1.addLeft(tree1.find(2), 5);

    auto morris = tree1.morrisInorder();
    auto pre = tree1.preOrder();
    auto in = tree1.inOrder();
    auto post = tree1.postOrder();
    auto level = tree1.levelOrder();

    auto println = [](const auto & container)
    {
        for (const auto& x : container) cout << x << " ";
        cout << endl;
    };

    println(pre);
    println(in);
    println(post);
    println(level);

    BinaryTree<int> tree2(pre.data(), in.data(), in.size(), 0);
    BinaryTree<int> tree3(in.data(), post.data(), in.size(), 1);
    BinaryTree<int> tree4(level.data(), in.data(), in.size(), 2);

    return 0;
}
