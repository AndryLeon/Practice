#ifndef _SLMGRAPH_H_
#define _SLMGRAPH_H_

#include "util.h"
#include "graph.h"
#include "lexiconGraph.h"
#include "languageModel.h"

#include <algorithm>
#include <deque>
#include <vector>
#include <sstream>
#include <string>


namespace pinyin{

class slmGraph:public graph{
public:
    slmGraph(lexiconGraph &le, languageModel &la, int solution_size)
            :graph(), lg(le), lm(la), solution_size(solution_size){
        std::cout << "Initializing the slmGraph\n";
        InitialVertexs(lg.edges_count+2);
        for(int i = 0; i < vertexs_count; ++i){
            vertexs[i]->data = new VertexData(solution_size);
        }

        for(int i = 0; i < lg.edges_count; ++i){
            EData* ed = (EData*)lg.edges[i]->data;
            int id = ed->id;
            VertexData* Vd = (VertexData*) vertexs[id]->data;
            Vd->phrase = ed->phrase;
            Vd->sy_sequence = ed->syllable_sequence;
        }
        int tsize = lg.vertexs[0]->tos.size();
        for(int i = 0; i < tsize; ++i){
            EData* ed = (EData*) lg.vertexs[0]->tos[i]->data;
            int id = ed->id;
            AddEdge(0, id, lm.getUnigram(ed->phrase));
        }
        int fsize = lg.vertexs[lg.vertexs_count-1]->froms.size();
        for(int i = 0; i < fsize; ++i){
            EData* ed = (EData*) lg.vertexs[lg.vertexs_count-1]->froms[i]->data;
            int id = ed->id;
            AddEdge(id, vertexs_count-1, 1);
        }
        VertexData* Vd = (VertexData*)vertexs[0]->data;
        Vd->phrase = "(s)";
        Vd = (VertexData*)vertexs[vertexs_count-1]->data;
        Vd->phrase = "(e)";
        for(int i = 0; i< lg.vertexs_count; ++i){
            int fsize = lg.vertexs[i]->froms.size();
            for(int j = 0; j < fsize; ++j){
                EData* ed1 = (EData*) lg.vertexs[i]->froms[j]->data;
                int prev_id = ed1->id;
                std::string prev_phrase = ed1->phrase;

                int tsize = lg.vertexs[i]->tos.size();
                for(int k = 0; k < tsize; ++k){
                    EData* ed2 = (EData*) lg.vertexs[i]->tos[k]->data;
                    int succ_id = ed2->id;
                    std::string succ_phrase = ed2->phrase;
                    AddEdge(prev_id, succ_id, lm.getBigram(prev_phrase, succ_phrase));
                }
            }
        }
        std::cout << "Done!\n";
    }

    void InitialVertexs(int n) override {
        for(int i = 0; i < n; ++i){
            vertexs.push_back(new Vertex(i));
        }
        vertexs_count = n;
    }

    Edge* AddEdge(int from, int to, double weight){
        Edge* e = new Edge(vertexs[from], vertexs[to]);
        e->from->tos.push_back(e);
        e->to->froms.push_back(e);
        edges.push_back(e);
        edges_count++;
        e->data = new EdgeData(calcualteWeight(weight));
        return e;
    }

    void calculatePath(Vertex* cur){
        VertexData* Vd = (VertexData*) cur->data;
        Vd->calculated = true;
        int fsize = cur->froms.size();
        for(int i = 0; i < fsize; ++i){
            Edge* e = cur->froms[i];
            EdgeData* Ed = (EdgeData*) e->data;
            double wg = Ed->weight;
            Vertex* vprev = e->from;
            VertexData* vprev_data = (VertexData*) vprev->data;

            if(!vprev_data->calculated)
                calculatePath(vprev);

            int setsize = vprev_data->dist.dlist.size();
            for(int j = 0; j < setsize; ++j){
                bool retval = Vd->dist.add(new Distance(vprev_data->dist.dlist[j]->distance+wg, e));/////
                if(!retval)
                    break;
            }
        }
    }

    void MakeSentence(Vertex* vcur, double new_probability){
        VertexData* Vd = (VertexData*)vcur->data;
        phrase_seq.push_front(Vd);

        int size = Vd->dist.dlist.size();

        for(int i = 0;i < size; ++i){
            Edge* prev = Vd->dist.dlist[i]->previous;
            if(prev){
                EdgeData* Ed = (EdgeData*) prev->data;
                MakeSentence(prev->from, new_probability+Ed->weight);
            }else{
                std::ostringstream sentence;
                std::vector<std::string> new_pinyin;

                int psize = phrase_seq.size();
                for(int j = 0; j < psize; ++j){
                    if(phrase_seq[j]->phrase == "")
                        continue;
                    sentence << phrase_seq[j]->phrase;
                    std::vector<std::string> v = phrase_seq[j]->sy_sequence.getSequence();
                    for(int k = 0; k < v.size(); ++k){
                        new_pinyin.push_back(v[k]);
                    }
                }
                std::string new_sentence = sentence.str();
                Solution new_solution;
                new_solution.sentence = new_sentence;
                new_solution.probability = new_probability;
                new_solution.pinyin = new_pinyin;
                sentences.push_back(new_solution);
            }
        }
        phrase_seq.pop_front();
    }

    std::vector<Solution> MakeSentence(){
        VertexData* vs_data = (VertexData*)vertexs[0]->data;
        vs_data->calculated = true;
        vs_data->dist.dlist.push_back(new Distance(0,nullptr));

        calculatePath(vertexs[vertexs_count-1]);
        MakeSentence(vertexs[vertexs_count-1], 0);

        sort(sentences.begin(), sentences.end(), [](const Solution &a, const Solution &b){return a.probability > b.probability;});
        std::vector<Solution> final_sentences;

        int size = sentences.size();
        for(int i = 0; i < size; ++i){
            bool duplicated = false;
            int fsize = final_sentences.size();
            for(int j = 0; j < fsize; ++j){
                if(sentences[i].sentence == final_sentences[j].sentence){
                    duplicated = true;
                    break;
                }
            }
            if(duplicated)
                continue;

            std::cout << sentences[i].sentence << " " << pow(10.0, sentences[i].probability) << "\n";

            final_sentences.push_back(sentences[i]);
            if(final_sentences.size() == solution_size)
                break;
        }
        return final_sentences;
    }

    double calcualteWeight(double weight){
        return log(weight);
    }

private:
    int solution_size;
    lexiconGraph &lg;
    languageModel &lm;

    std::deque<VertexData*> phrase_seq;
    std::vector<Solution> sentences;
};

}

#endif // _SLMGRAPH_H_
