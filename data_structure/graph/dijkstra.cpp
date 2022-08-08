
#include "graph_type.h"


// Dijkstra算法
template <class T>
void dijkstra(const MGraph<T>& graph, int start,
              std::vector<int>& pathArc,
              std::vector<int>& shortPath)
{
    pathArc.resize(graph.vexNum(), -1); // 前驱顶点下标
    shortPath.resize(graph.vexNum());
    std::vector<int> complete(graph.vexNum(), false);

    for (int j = 0; j < graph.vexNum(); ++j)
    {
        shortPath[j] = graph.arcs[start][j];
    }
    shortPath[start] = 0;
    complete[start] = true;
    // 递推求取最短路径
    for (int i = 1; i < graph.vexNum(); ++i)
    {
        int min = INT_MAX;
        int idx = -1;
        for (int j = 0; j < graph.vexNum(); ++j)
        {
            if (!complete[j] && shortPath[j] < min)
            {
                idx = j;
                min = shortPath[j];
            }
        }
        complete[idx] = true;
        // 修正当前的最短路径
        for (int j = 0; j < graph.vexNum(); ++j)
        {
            if (!complete[j] && graph.arcs[idx][j] < INT_MAX &&
                    min + graph.arcs[idx][j] < shortPath[j])
            {
                shortPath[j] = min + graph.arcs[idx][j];
                pathArc[j] = idx;
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
    int start = 2;
    vector<int> pathArc, shortPath;
    dijkstra(graph, start, pathArc, shortPath);

    for (int i = 0; i < graph.vexNum(); ++i)
    {
        printf("v%d - v%d: ", i, start);
        int j = i;
        while (pathArc[j] != -1)
        {
            printf("%d ", pathArc[j]);
            j = pathArc[j];
        }
        printf("\n");
    }
    printf("\nshortest path:\n");
    for (int i = 0; i < graph.vexNum(); ++i)
    {
        printf("v%d - v%d: %d\n", graph.vexs[start], graph.vexs[i], shortPath[i]);
    }
    return 0;
}
