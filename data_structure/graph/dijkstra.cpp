
#include "graph_type.h"


//Dijkstra算法
template <class T>
void dijkstra(const MGraph<T>& graph, int start,
              std::vector<int>& pathArc,
              std::vector<int>& shortPath)
{
    pathArc.resize(graph.vexNum(), -1);//前驱顶点下标
    shortPath.resize(graph.vexNum());
    std::vector<bool> final(graph.vexNum(), false);

    for (int j = 0; j < graph.vexNum(); ++j)
    {
        shortPath[j] = graph.arcs[start][j];
    }
    shortPath[start] = 0;
    final[start] = true;
    //递推求取最短路径
    for (int i = 1; i < graph.vexNum(); ++i)
    {
        int min = INT_MAX;
        int k = -1;
        for (int j = 0; j < graph.vexNum(); ++j)
        {
            if (!final[j] && shortPath[j] < min)
            {
                k = j;
                min = shortPath[j];
            }
        }
        final[k] = true;
        //修正当前的最短路径
        for (int j = 0; j < graph.vexNum(); ++j)
        {
            if (!final[j] && graph.arcs[k][j] < INT_MAX &&
                    min + graph.arcs[k][j] < shortPath[j])
            {
                shortPath[j] = min + graph.arcs[k][j];
                pathArc[j] = k;
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
        printf("v%d - v%d: ", start, i);
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
