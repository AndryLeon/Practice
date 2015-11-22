#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "util.h"

#include <vector>

namespace pinyin{

class graph{
public:
    graph():vertexs(0), edges(0), vertexs_count(0), edges_count(0){};

    virtual ~graph() = default;
    virtual void InitialVertexs(int n) = 0;
    //拷贝构造函数
//protected:
    int vertexs_count;
    int edges_count;
    std::vector<Vertex*> vertexs;
    std::vector<Edge*> edges;
};

}

#endif // _GRAPH_H_

