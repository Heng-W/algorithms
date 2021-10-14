
#include "graph_type.h"
#include <queue>
#include <iostream>


static std::vector<bool> visited;

template <class T>
void DFS(const MGraph<T>& graph, int i)
{
    visited[i] = true;
    std::cout << graph.vexs[i] << " ";
    for (int j = 0; j < graph.vexNum(); ++j)
    {
        if (graph.arcs[i][j] == 1 && !visited[j])
            DFS(graph, j);
    }
}

template <class T>
void BFS(const MGraph<T>& graph, int i)
{
    std::queue<int> vexQueue;
    visited[i] = true;
    std::cout << graph.vexs[i] << " ";
    vexQueue.push(i);
    while (!vexQueue.empty())
    {
        i = vexQueue.front();
        vexQueue.pop();
        for (int j = 0; j < graph.vexNum(); ++j)
        {
            if (graph.arcs[i][j] == 1 && !visited[j])
            {
                visited[j] = true;
                std::cout << graph.vexs[j] << " ";
                vexQueue.push(j);
            }
        }
    }
}

template <class T>
void DFS(const ALGraph<T>& graph, int i)
{
    visited[i] = true;
    std::cout << graph.adjList[i].data << " ";
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

template <class T>
void BFS(const ALGraph<T>& graph, int i)
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


template <class Graph>
void DFSTraverse(const Graph& graph)
{
    visited.resize(graph.vexNum());
    std::fill(visited.begin(), visited.end(), false);
    for (int i = 0; i < graph.vexNum(); ++i)
    {
        if (!visited[i]) DFS(graph, i);
    }
}

template <class Graph>
void BFSTraverse(const Graph& graph)
{
    visited.resize(graph.vexNum());
    std::fill(visited.begin(), visited.end(), false);
    for (int i = 0; i < graph.vexNum(); ++i)
    {
        if (!visited[i]) BFS(graph, i);
    }
}


int main()
{
    using namespace std;
    MGraph<char> mGraph({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'},
    {
        {0, 1}, {0, 5}, {1, 2}, {1, 8}, {1, 6},
        {2, 3}, {2, 8}, {3, 4}, {3, 6}, {3, 7},
        {3, 8}, {4, 5}, {4, 7}, {5, 6}, {6, 7}
    });
    DFSTraverse(mGraph);
    cout << endl;
    BFSTraverse(mGraph);
    cout << endl;

    ALGraph<char> aLGraph({'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i'},
    {
        {0, 1}, {0, 5}, {1, 2}, {1, 8}, {1, 6},
        {2, 3}, {2, 8}, {3, 4}, {3, 6}, {3, 7},
        {3, 8}, {4, 5}, {4, 7}, {5, 6}, {6, 7}
    });
    DFSTraverse(aLGraph);
    cout << endl;
    BFSTraverse(aLGraph);
    cout << endl;
    return 0;
}
