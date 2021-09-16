
#include "graph_type.h"
#include <algorithm>
#include <iostream>


//并查集查找
int find(std::vector<int>& parent, int x)
{
    if (parent[x] == 0) return x;
    return parent[x] = find(parent, parent[x]);
}

template <class T>
void kruskal(const MGraph<T>& graph)
{
    std::vector<int> parent(graph.vexNum()); //用来判断边是否形成环路
    std::vector<Arc> arcs;  //边集数组

    //构建边集数组
    for (int i = 0; i < graph.vexNum() - 1; ++i)
    {
        for (int j = i + 1; j < graph.vexNum(); ++j)
        {
            if (graph.arcs[i][j] < INT_MAX)
                arcs.push_back({i, j, graph.arcs[i][j]});
        }
    }
    //将边按照权值进行排序
    std::sort(arcs.begin(), arcs.end(), [](const auto& arc1, const auto& arc2)
    { return arc1.weight < arc2.weight; });

    for (int i = 0; i < graph.arcNum(); ++i)
    {
        int p1 = find(parent, arcs[i].begin);
        int p2 = find(parent, arcs[i].end);
        if (p1 != p2)
        {
            parent[p1] = p2;
            std::cout << arcs[i].begin << " - " << arcs[i].end << std::endl;
        }
    }
}


int main()
{
    using namespace std;
    MGraph<int> graph(vector<int>(9),
    {
        {0, 1, 10}, {0, 5, 11}, {1, 2, 18}, {1, 8, 12}, {1, 6, 16},
        {2, 8, 8}, {2, 3, 22}, {3, 8, 21}, {3, 6, 24}, {3, 7, 16},
        {3, 4, 20}, {4, 7, 7}, {4, 5, 26}, {5, 6, 17}, {6, 7, 19}
    });
    kruskal(graph);
    return 0;
}

