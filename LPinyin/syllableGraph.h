#ifndef _SYLLABLEGRAPH_H_
#define _SYLLABLEGRAPH_H_

#include "graph.h"
#include "syllableSequence.h"
#include "util.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace pinyin{

class syllableGraph:public graph {
public:
    syllableGraph(std::string str, pinyinSyllable &py):graph(), syllable(py){
        int len = str.length();
        std::cout << "Initializing syllableGraph \"" << str << "\"\n";
        InitialVertexs(len+1);

        for(int i = 0; i < len; ++i){
            for(int j = 1; j <= 6 && i+j <= len; ++j){
                std::string tmp = str.substr(i, j);
                if(syllable.isContains(tmp)){
                    Edge* e = AddEdge(vertexs[i], vertexs[i+j]);
                    e->data = new std::string(tmp);
                }
            }
        }
        BuildGraph();
        std::cout << "Done!\n";
    }

    void BuildGraph(){
        searchForward(vertexs[0]);
        VData* vd = (VData*)vertexs[vertexs_count-1]->data;
        if(!vd->accessedForward){
            std::cout << "Initial failed\n";
            return;
        }
        searchBackward(vertexs[vertexs_count-1]);
        shrinked_count = 0;
        for(int i = 0; i < vertexs_count; ++i){
            VData* vd = (VData*) vertexs[i]->data;
            if(vd->accessedForward && vd->accessedBackward){
                vd->shrinked_id = shrinked_count++;
            }
        }

        for(int i = 0; i < shrinked_count; ++i){
            Vertex* v = new Vertex(i);
            v->data = new VData();
            VData* vd = (VData*) v->data;
            vd->shrinked_id = i;
            shrinked_vertexs.push_back(v);
        }
        shrinked_edges_count = 0;
        for(int i = 0;i < vertexs_count; ++i){
            VData* vd1 = (VData*)vertexs[i]->data;
            if(vd1->shrinked_id >= 0){
                int tsize = vertexs[i]->tos.size();
                for(int j = 0; j < tsize; ++j){
                    VData* vd2 = (VData*)vertexs[i]->tos[j]->to->data;
                    if(vd2->shrinked_id >= 0){
                        Edge* e = AddEdge(shrinked_vertexs[vd1->shrinked_id], shrinked_vertexs[vd2->shrinked_id]);
                        std::string* str = (std::string*) vertexs[i]->tos[j]->data;
                        e->data = new std::string(*(str));
                        shrinked_edges_count++;
                    }
                }
            }
        }
    }

    void searchForward(Vertex* v){
        int tsize = v->tos.size();
        VData* vd = (VData*) v->data;
        vd->accessedForward = true;
        for(int i = 0; i < tsize; ++i){
            vd = (VData*) v->tos[i]->to->data;
            if(!vd->accessedForward)
                searchForward(v->tos[i]->to);
        }
    }

    void searchBackward(Vertex* v){
        int fsize = v->froms.size();
        VData* vd = (VData*) v->data;
        vd->accessedBackward = true;
        for(int i = 0; i < fsize; ++i){
            vd = (VData*) v->froms[i]->from->data;
            if(!vd->accessedBackward)
                searchBackward(v->froms[i]->from);
        }
    }

    void InitialVertexs(int n) override {
        for(int i = 0; i < n; ++i){
            vertexs.push_back(new Vertex(i));
        }
        for(int i = 0; i < n; ++i){
            vertexs[i]->data = new VData;
        }
        vertexs_count = n;
    }

    Edge* AddEdge(Vertex* from, Vertex* to) {
        Edge* e = new Edge(from, to);
        e->from->tos.push_back(e);
        e->to->froms.push_back(e);
        edges.push_back(e);
        edges_count++;
        return e;
    }

    ~syllableGraph(){
        for(int i = 0; i < vertexs_count; ++i){
            VData* vd = (VData*) vertexs[i]->data;
            delete vd;
            delete vertexs[i];
        }
        for(int i = 0; i < shrinked_count; ++i){
            VData* vd = (VData*) shrinked_vertexs[i]->data;
            delete vd;
            delete shrinked_vertexs[i];
        }
        for(int i = 0; i < edges_count; ++i){
            std::string* str = (std::string*) edges[i]->data;
            delete str;
            delete edges[i];
        }
    }

    std::vector<syllableSequence> getSequences(int depth){
        if(!sss.empty())
            sss.clear();
        if(!ss.empty())
            ss.clear();
        for(int i = 0;i < shrinked_count; ++i){
            getSequenceRec(shrinked_vertexs[i], depth, i);
        }
        return sss;
    }

    void getSequenceRec(Vertex* cur, int depth, int from){
        int tsize = cur->tos.size();
        for(int i = 0; i < tsize; ++i){
            std::string* vd = (std::string*) cur->tos[i]->data;
            ss.push_back(*vd);
            sss.push_back(syllableSequence(ss, from, cur->tos[i]->to->id));
            if(depth > 1){
                getSequenceRec(cur->tos[i]->to, depth-1, from);
            }
            ss.pop_back();
        }
    }

    std::string toString(){
        std::ostringstream ss;
        ss << "The syllable graph is : \n";
        std::vector<syllableSequence> tmp = getSequences(2);
        int tsize = tmp.size();
        for(int i = 0;i < tsize; ++i){
            std::vector<std::string> v = tmp[i].getSequence();
            int vsize = v.size();
            for(int j = 0;j < vsize; ++j){
                ss << v[j] << " " ;
            }
            ss << tmp[i].getFrom() << " " << tmp[i].getTo() << "\n";
        }
        ss << "\n";
        return ss.str();
    }

    int getShrinked_count(){
        return shrinked_count;
    }

private:
    int shrinked_count;
    int shrinked_edges_count;
    std::vector<Vertex*> shrinked_vertexs;
    pinyinSyllable &syllable;
    static const int max_syllable_length = 6;

    std::vector<syllableSequence> sss;
    std::vector<std::string> ss;

};

}

#endif // _SYLLABLEGRAPH_H_
