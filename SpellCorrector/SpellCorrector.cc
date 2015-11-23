#include "SpellCorrector.h"

#include <fstream>
#include <algorithm>
#include <utility>
#include <iostream>

namespace spell{

void SpellCorrector::load(std::string fileName){
    std::ifstream file(fileName, std::ifstream::binary|std::ifstream::in);
    if(!file){
        std::cout << "Can not find file\n";
        return;
    }

    file.seekg(0, std::ifstream::end);
    int length = file.tellg();
    file.seekg(0, std::ifstream::beg);

    std::string line(length, '0');

    file.read(&line[0], length);

    transform(line.begin(), line.end(), line.begin(), [](char& ch)-> char{if(isalpha(ch)) return tolower(ch);return '-';});

    std::string::size_type beg;
    std::string::size_type end = line.size();

    for(std::string::size_type i = 0; ;){
        while(line[i] == '-' && i < end) i++;

        if(i >= end) break;

        beg = i;

        while(line[i] != '-' && i < end) i++;

        dict[line.substr(beg, i - beg)]++;
    }
}

std::string SpellCorrector::correct(std::string text){
    Vector tmpV;
    Dictionary tmpD;

    edit(text, tmpV);
    isValid(tmpV, tmpD);

    if(tmpD.size() > 0)
        return max_element(tmpD.begin(), tmpD.end(), [](const std::pair<std::string, int>& left, const std::pair<std::string, int>& right){return left.second < right.second;})->first;

    return "";
}

void SpellCorrector::edit(std::string &text, Vector &v){
    int size = text.size();
    typedef std::string::size_type st;
    for(st i = 0; i < size; ++i)
        v.push_back(text.substr(0, i) + text.substr(i+1));

    for(st i = 0; i < size-1; ++i)
        v.push_back(text.substr(0, i) + text[i+1] + text[i] + text.substr(i+2));

    for(char j = 'a'; j < 'z'; ++j){
        for(st i = 0; i < size+1; ++i)
             v.push_back(text.substr(0, i) + j + text.substr(i));
         for(st i = 0; i < size; ++i)
            v.push_back(text.substr(0, i) + j + text.substr(i+1));
    }
}

void SpellCorrector::isValid(Vector &v, Dictionary &d){
    Dictionary::iterator end = dict.end();

    int vsize = v.size();
    for(int i = 0; i < vsize; ++i){
        Dictionary::iterator iter = dict.find(v[i]);

        if(iter != end)
            d[iter->first] = iter->second;
    }
}

}
