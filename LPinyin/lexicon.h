#ifndef _LEXICON_H_
#define _LEXICON_H_

#include "util.h"

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace pinyin{

class lexicon{
public:
    lexicon(std::string fileName, pinyinSyllable &py):lexicons(), pySyllable(py), max_length(0), root(new Node()){
        std::ifstream file(fileName);
        if(!file){
            std::cout << "Unable to open " << fileName << " file\n";
            return;
        }
        std::cout << "Initializing the " << fileName << " file\n";
        std::string str;
        while(getline(file, str)){
            parseLine(str);
        }
        file.close();
        std::cout << "Done!\n";
        return;
    }

    ~lexicon(){
        deconstruct(root);
    }

    void deconstruct(Node* cur){
        for(auto iter = cur->dict.begin(); iter != cur->dict.end(); ++iter){
            deconstruct(iter->second);
        }
        delete cur;
        return;
    }

    bool isContains(std::string lexicon){
        return lexicons.find(lexicon) != lexicons.end();
    }

    int getMaxlength(){
        return max_length;
    }

    std::string toString(){
        std::ostringstream ss;
        ss << "The lexicon includes :\n";
    //    for(auto iter = lexicons.cbegin(); iter != lexicons.cend(); ++iter){
   //         ss << *iter << " ";
   //     }
        ss << "\n";
        ss << "The max length is : " << max_length << "\n";
        return ss.str();
    }

    std::vector<std::string> getPhrases(std::vector<std::string> pinyins){
        return getPhrasesRec(root, pinyins, 0);
    }

    std::vector<std::string> getPhrasesRec(Node* cur, std::vector<std::string> pinyins, int index){
        std::string pinyin = pinyins[index];

        Node* next;

        if(cur->dict.find(pinyin) == cur->dict.end()){
            std::cout << "Can not get proper phrases\n";
            return std::vector<std::string>();
        }else{
            next = cur->dict.at(pinyin);
        }

        if(index == pinyins.size()-1){
            return next->phrases;
        }
        return getPhrasesRec(next, pinyins, index+1);
    }

private:
    void parseLine(std::string str){
        std::size_t sep_pos = str.find('\t');
        std::string phrase = str.substr(sep_pos + 1);
        std::vector<std::string> pinyins = SplitString(str.substr(0, sep_pos),'\'');
        insertPhrase(root, pinyins, 0, phrase);
    }

    void insertPhrase(Node* cur, std::vector<std::string> &pinyins, int index, std::string phrase){
        std::string pinyin = pinyins[index];
        if(!pySyllable.isContains(pinyin)){
            std::cout << "Invalid syllable \"" << pinyin << "\"\n";
            return;
        }

        Node* next;
        if(cur->dict.find(pinyin) != cur->dict.end()){
            next = cur->dict[pinyin];
        }else{
            next = cur->dict[pinyin] = new Node();
        }

        if(index == pinyins.size()-1){
            next->phrases.push_back(phrase);
            lexicons.insert(phrase);
            if(max_length < pinyins.size())
                max_length = pinyins.size();
            return;
        }
        insertPhrase(next, pinyins, index+1, phrase);
    }

private:
    std::set<std::string> lexicons;
    pinyinSyllable &pySyllable;
    int max_length;
    Node* root;

};

}

#endif // _LEXICON_H_
