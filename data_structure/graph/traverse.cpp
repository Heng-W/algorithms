
#include "graph_type.h"
#include <queue>
#include <iostream>

constexpr int MAX_VEC = 100;

static bool visited[MAX_VEC];

template <class Vertex>
void DFS(const MGraph<Vertex>& graph, int i)
{    
    visited[i] = true;
    std::cout << graph.vexs[i] <<" ";
    for (int j = 0; j < graph.vexNum; ++j)
    {
        if (graph.arcs[i][j] == 1 && !visited[j])
            DFS(graph, j);
    }
}

template <class Vertex>
void BFS(const MGraph<Vertex>& graph, int i)
{
    std::queue<int> vexQueue;
    visited[i] = true;
    std::cout << vertex[i] << " ";
    vexQueue.push(i);
    while (!vexQueue.empty())
    {
        i = vexQueue.front();
        vexQueue.pop();
        for (int j = 0; j < graph.vexNum; ++j)
        {
            if (graph.arcs[i][j] == 1 && !visited[j])
            {
                visited[j] = true;
                std::cout << vertex[j] << " ";
                vexQueue.push(j);
            }
        }
    }
}

template <class Vertex>
void DFS(const ALGraph<Vertex>& graph, int i)
{    
    visited[i] = true;
    std::cout << graph.vexs[i] <<" ";
    const auto* cur = graph.adjList[i].firstArc;
    while (cur)
    {
        if (!visited[cur->adjvex])
        {
            DFS(graph, cur->adjvex);
        }
        cur = cur->next;
    }
}

template <class Vertex>
void BFS(const ALGraph<Vertex>& graph, int i)
{
    std::queue<int> vexQueue;
    visited[i] = true;
    std::cout << graph.adjList[i].data << " ";
    vexQueue.push(i);
    while (!vexQueue.empty())
    {
        i = vexQueue.front();
        vexQueue.pop();
        const auto* cur = graph.adjList[i].firstArc;
        while (cur)
        {
            int j = cur->adjvex;
            if (!visited[j])
            {
                visited[j] = true;
                std::cout << graph.adjList[j].data << " ";
                vexQueue.push(j);
            }
            cur = cur->next;
        }
    }
}
