#ifndef GRAPH_TYPE_H
#define GRAPH_TYPE_H

#include <limits.h>
#include <vector>

struct Arc
{
    int begin;
    int end;
    int weight = 1;
};

/** 邻接矩阵表示  **/
template <class Vertex>
struct MGraph
{
    using VertexType = Vertex;
    using VertexArray = std::vector<VertexType>;
    using ArcMat = std::vector<std::vector<int>>;


    VertexArray vexs;
    ArcMat arcs;
    int _arcNum;

    // 构造无向图
    MGraph(const VertexArray& _vexs,
           const std::vector<Arc>& _arcs)
        : vexs(_vexs),
          _arcNum(_arcs.size())
    {
        arcs.resize(vexs.size(), std::vector<int>(vexs.size(), INT_MAX));
        for (int i = 0; i < (int)arcs.size(); ++i)
        {
            arcs[i][i] = 0;
        }
        for (int k = 0; k < (int)_arcs.size(); ++k)
        {
            int i = _arcs[k].begin;
            int j = _arcs[k].end;
            arcs[i][j] = _arcs[k].weight;
            arcs[j][i] = arcs[i][j];
        }
    }

    int vexNum() const { return vexs.size(); }
    int arcNum() const { return _arcNum; }
};

/** 邻接表  **/
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
    ArcNode* firstArc;

    VertexNode(const VertexType& _data)
        : data(_data), firstArc(nullptr) {}

    VertexNode(VertexType&& _data)
        : data(std::move(_data)), firstArc(nullptr) {}
};


template <class Vertex>
struct ALGraph
{
    using VertexType = Vertex;
    using AdjList = std::vector<VertexNode<Vertex>>;

    AdjList adjList;
    int _arcNum;

    // 构造无向图
    ALGraph(const std::vector<VertexType>& _vexs,
            const std::vector<Arc>& _arcs)
        : _arcNum(_arcs.size())
    {
        adjList.reserve(_vexs.size());
        for (const auto& vex : _vexs)
            adjList.emplace_back(vex);

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

            arc = new ArcNode();
            arc->adjvex = i;
            arc->weight = weight;
            arc->next = adjList[j].firstArc;
            adjList[j].firstArc = arc;
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

#endif // GRAPH_TYPE_H
