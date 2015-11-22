#ifndef _PINYINSYLLABLE_H_
#define _PINYINSYLLABLE_H_

#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <string>

namespace pinyin{

class pinyinSyllable{
public:
    pinyinSyllable(std::string fileName){
        std::ifstream file(fileName);
        if(!file){
            std::cout << "Unable to open " << fileName << " file\n";
            return;
        }
        std::cout << "Initializing the " << fileName << " file\n";
        std::string str;
        while(getline(file, str)){
            syllableSet.insert(str);
        }
        file.close();
        std::cout << "Done!\n";
    }

    bool isContains(std::string str){
        return syllableSet.find(str) != syllableSet.end();
    }

    std::string toString(){
        std::ostringstream ss;
        for(auto iter = syllableSet.cbegin();iter != syllableSet.cend(); ++iter)
            ss << "\"" << *iter << "\", ";
        ss << "\n";
        return ss.str();
    }

private:
    std::set<std::string> syllableSet;
};

}

#endif // _PINYINSYLLABLE_H_
