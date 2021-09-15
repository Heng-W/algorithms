
#include "graph_type.h"
#include <iostream>

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
        //找到权值最小的边
        for (int j = 1; j < graph.vexNum(); ++j)
        {
            if (lowcost[j] != 0 && lowcost[j] < min)
            {
                min = lowcost[j];
                index = j;
            }
        }
        std::cout << adjvex[index] << " - " << index <<std::endl;
        lowcost[index] = 0; //标记完成
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
