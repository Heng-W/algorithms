
#include <vector>
#include <stack>
#include <iostream>


// 边表
struct ArcNode
{
    int adjvex; // 邻接序号
    int weight = 1;
    ArcNode* next;
};

// 顶点表
template <class Vertex>
struct VertexNode
{
    using VertexType = Vertex;

    VertexType data;
    int in; // 入度
    ArcNode* firstArc;

    VertexNode(const VertexType& _data)
        : data(_data), in(0), firstArc(nullptr) {}

    VertexNode(VertexType&& _data)
        : data(std::move(_data)), in(0), firstArc(nullptr) {}
};


template <class Vertex>
struct ALGraph
{
    using VertexType = Vertex;
    using AdjList = std::vector<VertexNode<Vertex>>;

    struct Arc
    {
        int begin;
        int end;
        int weight = 1;
    };

    AdjList adjList;
    int _arcNum;

    // 构造无向图
    ALGraph(const std::vector<VertexType>& _vexs,
            const std::vector<Arc>& _arcs)
        : _arcNum(_arcs.size())
    {
        adjList.reserve(_vexs.size());
        for (const auto& vex : _vexs)
        {
            adjList.emplace_back(vex);
        }

        for (int k = 0; k < (int)_arcs.size(); ++k)
        {
            int i = _arcs[k].begin;
            int j = _arcs[k].end;
            int weight = _arcs[k].weight;

            ArcNode* arc = new ArcNode();
            arc->adjvex = j;
            arc->weight = weight;
            arc->next = adjList[i].firstArc;
            adjList[i].firstArc = arc;
            ++adjList[j].in;
        }
    }

    ~ALGraph()
    {
        for (int i = 0; i < (int)adjList.size(); ++i)
        {
            ArcNode* cur = adjList[i].firstArc;
            while (cur)
            {
                ArcNode* next = cur->next;
                delete cur;
                cur = next;
            }
        }
    }

    int vexNum() const { return adjList.size(); }
    int arcNum() const { return _arcNum; }
};


template <class T>
bool topologicalSort(ALGraph<T>& graph)
{
    std::stack<int> sta;
    for (int i = 0; i < graph.vexNum(); ++i)
    {
        if (graph.adjList[i].in == 0)
        {
            sta.push(i);
        }
    }
    int count = 0;
    while (!sta.empty())
    {
        int index = sta.top();
        sta.pop();
        std::cout << graph.adjList[index].data << " -> ";
        ++count;
        ArcNode* cur = graph.adjList[index].firstArc;
        while (cur)
        {
            int i = cur->adjvex;
            if (--graph.adjList[i].in == 0)
            {
                sta.push(i);
            }
            cur = cur->next;
        }
    }
    return count >= graph.vexNum();
}


// 测试
int main()
{
    using namespace std;
    vector<int> vexs;
    for (int i = 0; i < 14; ++i) vexs.push_back(i);
    ALGraph<int> aLGraph(vexs,
    {
        {0, 4}, {0, 5}, {0, 11}, {1, 2}, {1, 4},
        {1, 8}, {2, 5}, {2, 6}, {2, 9}, {3, 2},
        {3, 13}, {4, 7}, {5, 8}, {5, 12}, {6, 5},
        {8, 7}, {9, 10}, {9, 11}, {10, 13}, {12, 9}
    });
    topologicalSort(aLGraph);
    cout << endl;
    return 0;
}
