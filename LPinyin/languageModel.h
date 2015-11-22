#ifndef _LANGUAGEMODEL_H_
#define _LANGUAGEMODEL_H_

#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

namespace pinyin{

class languageModel{
public:
    languageModel(std::string fileName, lexicon &lx):lx(lx){
        std::ifstream file(fileName);
        if(!file){
            std::cout << "Can not open " << fileName << "\n";
            return;
        }
        std::cout << "Initilizing language model\n";
        std::string str;
        while(getline(file, str)){
            if(parseLine(str))
                break;
        }
        file.close();
        std::cout << "Done!\n";
    }

    std::string toString(){
        std::ostringstream ss;
        ss << "The languageModel dictionary size is " << unigram.size() << " and " << bigram.size() << "\n";
        auto tmp = unigram.begin();
        ss << unigram.begin()->second << "\n";
        ss << (bigram.begin()->second).begin()->second << "\n";
        return ss.str();
    }

    double getUnigram(std::string phrase){
        if(unigram.find(phrase) != unigram.end())
            return unigram[phrase];
        return infinitesimal;
    }

    double getBigram(std::string phrase1, std::string phrase2){
        double delta = 0.0;
        if(bigram.find(phrase1)!= bigram.end()){
            if (bigram[phrase1].find(phrase2) != bigram[phrase1].end())
                return bigram[phrase1][phrase2];
            else if(bigram[phrase1].find(unknown) != bigram[phrase1].end())
                delta = bigram[phrase1][unknown];
        }else if(bigram.find(unknown) != bigram.end()){
            if(bigram[unknown].find(phrase2) != bigram[unknown].end())
                delta = bigram[unknown][phrase2];
        }
        return getUnigram(phrase1) * getUnigram(phrase2) * (2.7 + delta);
    }

private:
    bool parseLine(std::string str){
        if(stage == 0){
            if(str.substr(0, 8) == "\\1-gram\\")
                stage++;
        }else if(stage == 1){
            if(str.substr(0, 8) == "\\2-gram\\"){
                stage++;
            }else{//unigram
                std::size_t _pos = str.find(' ');
                std::string phrase = str.substr(0, _pos);
                double probability = atof(str.substr(_pos + 3, str.find(' ', _pos + 3) - (_pos + 3)).c_str());
                if (!lx.isContains(phrase))
                    return false;

                unigram[phrase] = probability;
            }
        }else if(stage == 2){
            if(str.substr(0, 8) == "\\3-gram\\"){
                return true;
            }else{//bigram
                int _pos1 = str.find(' ');
                std::string phrase1 = str.substr(0, _pos1);
                int _pos2 = str.find(' ', _pos1 + 1);
                std::string phrase2 = str.substr(_pos1 + 1, _pos2 - _pos1 - 1);
                double probability = atof(str.substr(_pos2 + 3, str.find(' ', _pos2 + 3) - (_pos2 + 3)).c_str());
                if(phrase1 != unknown && !lx.isContains(phrase1))
                    return false;
                if (phrase2 != unknown && !lx.isContains(phrase2))
                    return false;

                bigram[phrase1][phrase2] = probability;
            }
        }
        return false;
    }

private:
    std::map<std::string, double> unigram;
    std::map<std::string, std::map<std::string, double> > bigram;
    int stage = 0;
    lexicon &lx;
    const std::string unknown = "<unknown>";
    const double infinitesimal = 1e-100;
};

}

#endif // _LANGUAGEMODEL_H_
