#ifndef GRAPH_TYPE_H
#define GRAPH_TYPE_H

/** 邻接矩阵表示  **/
template <class Vertex, int MAX_VEX = 100>
struct MGraph
{
    using VertexType = Vertex;

    VertexType vexs[MAX_VEX];
    int arcs[MAX_VEX][MAX_VEX];
    int vexNum, arcNum;
};

/** 邻接表  **/
//边表
struct ArcNode
{
    int adjvex;
    ArcNode* next;
};

//顶点表
template <class Vertex>
struct VertexNode
{
    using VertexType = Vertex;

    VertexType data;
    ArcNode* firstArc;
};


template <class Vertex, int MAX_VEX = 100>
struct ALGraph
{
    VertexNode<Vertex> adjList[MAX_VEX];
    int vexNum, arcNum;
};

#endif //GRAPH_TYPE_H
