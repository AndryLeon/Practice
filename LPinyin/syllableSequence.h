#ifndef _SYLLABLESEQUENCE_H_
#define _SYLLABLESEQUENCE_H_

#include <string>
#include <vector>

namespace pinyin{

class syllableSequence{
public:
    syllableSequence(std::vector<std::string> sy, int from, int to):sequences(sy), from(from), to(to){}

    syllableSequence(){}

    std::vector<std::string> getSequence(){
        return sequences;
    }

    int getFrom(){
        return from;
    }

    int getTo(){
        return to;
    }

private:
    std::vector<std::string> sequences;
    int from, to;
};

}

#endif // _SYLLABLESEQUENCE_H_
