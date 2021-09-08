
#include <vector>

//并查集
class DisjointSet
{
public:

    DisjointSet(int size)
    {
        root_.resize(size);
        for (int i = 0; i < size; ++i)
            root_[i] = i;
    }

    void join(int x, int y)
    {
        int root1 = find(x);
        int root2 = find(y);
        if (root1 != root2)
            root_[root1] = root2;
    }

    int find(int x)
    {
        int root = x;
        while (root != root_[root]) root = root_[root];
        //路径压缩优化
        while (root != x)
        {
            int tmp = root_[x];
            root_[x] = root;
            x = tmp;
        }
        return root;
    }

    bool isConnected(int x, int y)
    { return find(x) == find(y); }

private:
    std::vector<int> root_;
};


#include <iostream>

int main()
{
    using namespace std;
    DisjointSet set(10);
    set.join(0, 1);
    set.join(3, 1);
    set.join(2, 4);

    cout << set.find(3) << endl;

    cout << set.isConnected(0, 3) << endl;
    cout << set.isConnected(0, 2) << endl;

    return 0;
}


