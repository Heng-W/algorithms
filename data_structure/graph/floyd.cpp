
#include "graph_type.h"

// 最短路径，Floyd算法
template <class T>
void floyd(const MGraph<T>& graph,
           std::vector<std::vector<int>>& pathArc,
           std::vector<std::vector<int>>& shortPath)
{
    const int vexNum = graph.vexNum();
    pathArc.resize(vexNum, std::vector<int>(vexNum)); // 前驱顶点下标
    shortPath.resize(vexNum, std::vector<int>(vexNum));

    for (int i = 0; i < vexNum; ++i)
    {
        for (int j = 0; j < vexNum; ++j)
        {
            shortPath[i][j] = graph.arcs[i][j];
            pathArc[i][j] = j;
        }
    }
    for (int k = 0; k < vexNum; ++k)
    {
        for (int i = 0; i < vexNum; ++i)
        {
            for (int j = 0; j < vexNum; ++j)
            {
                if (shortPath[i][k] < INT_MAX && shortPath[k][j] < INT_MAX &&
                        shortPath[i][j] > shortPath[i][k] + shortPath[k][j])
                {
                    shortPath[i][j] = shortPath[i][k] + shortPath[k][j];
                    pathArc[i][j] = pathArc[i][k];
                }
            }
        }
    }
}


#include <cstdio>

int main()
{
    using namespace std;
    vector<int> vexs;
    for (int i = 0; i < 9; ++i) vexs.push_back(i);
    MGraph<int> graph(vexs,
    {
        {0, 1, 1}, {0, 2, 5}, {1, 2, 3}, {1, 3, 7}, {1, 4, 5},
        {2, 4, 1}, {2, 5, 7}, {3, 4, 2}, {3, 6, 3}, {4, 5, 3},
        {4, 6, 6}, {4, 7, 9}, {5, 7, 5}, {6, 7, 2}, {6, 8, 7},
        {7, 8, 4}
    });
    vector<vector<int>> pathArc, shortPath;
    floyd(graph, pathArc, shortPath);

    for (int i = 0; i < graph.vexNum(); ++i)
    {
        for (int j = i + 1; j < graph.vexNum(); ++j)
        {
            printf("v%d-v%d: %d ", i, j, shortPath[i][j]);
            int k = pathArc[i][j];
            printf(" path: %d", i);
            while (k != j)
            {
                printf(" -> %d", k);
                k = pathArc[k][j];
            }
            printf(" -> %d\n", j);
        }
        printf("\n");
    }
    return 0;
}
