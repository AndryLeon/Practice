#ifndef _LEXICONGRAPH_H_
#define _LEXICONGRAPH_H_

#include "util.h"

#include <iostream>
#include <sstream>
#include <vector>

namespace pinyin{

class lexiconGraph:public graph{
public:
    lexiconGraph(syllableGraph &sy, lexicon &le, int depth):graph(), sg(sy), lx(le){
        int n = sg.getShrinked_count();
        std::cout << "Initializing lexicon Graph\n";
        InitialVertexs(n);
        std::vector<syllableSequence> tmp = sg.getSequences(depth);
        int vsize = tmp.size();
        for(int i = 0; i < vsize; ++i){
            auto v = lx.getPhrases(tmp[i].getSequence());
            int size = v.size();
            for(int j = 0; j < size; ++j){
                Edge* e = AddEdge(vertexs[tmp[i].getFrom()], vertexs[tmp[i].getTo()]);
                e->data = new EData(v[j], edges_count, tmp[i]);
                std::cout << v[j] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "Done!\n";
    }

    void InitialVertexs(int n) override {
        for(int i = 0; i < n; ++i){
            vertexs.push_back(new Vertex(i));
        }
        vertexs_count = n;
    }

    Edge* AddEdge(Vertex* from, Vertex* to){
        Edge* e = new Edge(from, to);
        e->from->tos.push_back(e);
        e->to->froms.push_back(e);
        edges.push_back(e);
        edges_count++;
        return e;
    }

    ~lexiconGraph(){
        for(int i = 0; i < edges_count; ++i){
            EData* ed = (EData*) edges[i]->data;
            delete ed;
            delete edges[i];
        }
    }

    std::string toString(){
        std::ostringstream ss;
        for(int i = 0; i < edges_count; ++i){
            EData* ed = (EData*)edges[i]->data;
            ss << "id : " << ed->id
                << " from : " << ed->phrase << "\n";
        }
        return ss.str();
    }

private:
    syllableGraph &sg;
    lexicon &lx;
};

}

#endif // _LEXICONGRAPH_H_
