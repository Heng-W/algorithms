
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
        rank_.resize(size, 1);
    }

    void join(int x, int y)
    {
        int root1 = find(x);
        int root2 = find(y);
        if (root1 != root2)
        {
            if (rank_[root1] < rank_[root2])
            {
                root_[root1] = root2;
            }
            else if (rank_[root2] < rank_[root1])
            {
                root_[root2] = root1;
            }
            else
            {
                root_[root1] = root2;
                ++rank_[root2];
            }
        }

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

    //递归写法
    int findR(int x)
    {
        if (x == root_[x]) return x;
        return root_[x] = findR(root_[x]);
    }

    bool isConnected(int x, int y)
    { return find(x) == find(y); }

private:
    std::vector<int> root_;
    std::vector<int> rank_;
};


#include <iostream>

int main()
{
    using namespace std;
    DisjointSet set(10);
    set.join(0, 1);
    set.join(3, 1);
    set.join(8, 3);
    set.join(3, 7);
    set.join(2, 4);
    set.join(2, 5);

    //cout << set.find(3) << endl;
    cout << set.findR(3) << endl;

    cout << set.isConnected(0, 3) << endl;
    cout << set.isConnected(0, 2) << endl;

    return 0;
}


