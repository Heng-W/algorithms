
#include "graph_type.h"
#include <iostream>

// 最小生成树，prim算法
template <class T>
void prim(const MGraph<T>& graph)
{
    std::vector<int> adjvex(graph.vexNum());
    std::vector<int> lowcost(graph.vexNum());

    for (int j = 1; j < graph.vexNum(); ++j)
    {
        lowcost[j] = graph.arcs[0][j];
        adjvex[j] = 0;
    }
    for (int i = 1; i < graph.vexNum(); ++i)
    {
        int min = INT_MAX;
        int index = 0;
        // 找到权值最小的边
        for (int j = 1; j < graph.vexNum(); ++j)
        {
            if (lowcost[j] != 0 && lowcost[j] < min)
            {
                min = lowcost[j];
                index = j;
            }
        }
        std::cout << adjvex[index] << " - " << index << std::endl;
        lowcost[index] = 0; // 标记完成
        for (int j = 1; j < graph.vexNum(); ++j)
        {
            if (lowcost[j] != 0 && graph.arcs[index][j] < lowcost[j])
            {
                lowcost[j] = graph.arcs[index][j];
                adjvex[j] = index;
            }
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
    prim(graph);
    return 0;
}
