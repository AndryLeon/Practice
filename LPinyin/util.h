#ifndef _UTIL_H_
#define _UTIL_H_

#include "syllableSequence.h"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace pinyin{

    struct Node{
        Node() = default;
        std::vector<std::string> phrases;
        std::map<std::string, Node*> dict;
    };

    struct Edge;

    struct Vertex{
        Vertex(int i):id(i){}

        std::vector<Edge* > froms;
        std::vector<Edge* > tos;
        int id;
        void* data;
    };

    struct Edge{
        Edge(Vertex* from, Vertex* to):from(from), to(to){}

        Vertex* from;
        Vertex* to;
        void* data;
    };

    struct VData{
        bool accessedForward = false;
        bool accessedBackward = false;
        int shrinked_id = -1;
    };

    struct EData{
        EData(std::string phrase, int id, syllableSequence syllable_sequence)
                                :phrase(phrase), id(id), syllable_sequence(syllable_sequence) {}

        std::string phrase;
        int id;
        syllableSequence syllable_sequence;
    };

    struct Distance{
        double distance = -1*1e50;
        Edge* previous = nullptr;

        Distance(double distance, Edge* previous):distance(distance), previous(previous){}
    };

    struct distanceSet{
        std::vector<Distance*> dlist;
        int limit;

        distanceSet(int limit):limit(limit){}

        bool add(Distance* new_dist){
            int size = dlist.size();
            int i = 0;
            bool added = false;
            while(i < size){
                if(new_dist->distance > dlist[i]->distance){
                    dlist.push_back(new_dist);
                    for(int j = size;j > i; j--){
                        dlist[j] = dlist[j-1];
                    }
                    dlist[i] = new_dist;
                    if(dlist.size() > limit)
                        dlist.pop_back();
                    added = true;
                    break;
                }
                i++;
            }
            if(!added){
                if(dlist.size() == limit)
                    return false;
                dlist.push_back(new_dist);
            }
            return true;
        }
    };

    struct EdgeData{
        double weight = 0;
        EdgeData(double wg):weight(wg){}
    };

    struct VertexData{
        std::string phrase = "";
        syllableSequence sy_sequence;
        bool calculated = false;
        distanceSet dist;

        VertexData(int solution_size):dist(distanceSet(solution_size)), sy_sequence(){}
    };

    struct Solution{
    friend bool operator==(const Solution &a, const Solution &b);

        std::string sentence;
        std::vector<std::string> pinyin;
        double probability;

        std::string getPinyinString(){
            std::ostringstream ss;
            int size = pinyin.size();
            for(int i = 0;i < size; ++i){
                ss << pinyin[i];
                if(i != size-1)
                    ss << ' ';
            }
            return ss.str();
        }
    };

    bool operator==(const Solution &a, const Solution &b){
        if(a.sentence == b.sentence && a.probability == b.probability && a.pinyin.size() == b.pinyin.size()){
            int asize = a.pinyin.size();
            for(int i = 0; i < asize; ++i){
                if(a.pinyin[i] != b.pinyin[i])
                    return false;
            }
            return true;
        }
        return false;
    }

    std::vector<std::string> SplitString(const std::string& s,  const char& c){
        std::vector<std::string> v;
        std::size_t pos1, pos2;
        pos2 = s.find(c);
        pos1 = 0;

        while(pos2 != std::string::npos){
            v.push_back(s.substr(pos1, pos2-pos1));
            pos1 = pos2 + 1;
            pos2 = s.find(c, pos1);
        }

        if(pos1 != s.length())
            v.push_back(s.substr(pos1));
        return v;
    }

}

#endif // _UTIL_H_
