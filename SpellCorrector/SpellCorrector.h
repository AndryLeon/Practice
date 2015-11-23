// C++ implement of http://norvig.com/spell-correct.html


#ifndef _SPELLCORRECTOR_H_
#define _SPELLCORRECTOR_H_

#include <string>
#include <vector>
#include <unordered_map>

namespace spell{

class SpellCorrector{
private:
typedef std::vector<std::string> Vector;
typedef std::unordered_map<std::string, int> Dictionary;

    void edit(std::string&, Vector&);
    void isValid(Vector&, Dictionary&);

    Dictionary dict;
public:
    SpellCorrector() = default;
    void load(std::string fileName);
    std::string correct(std::string);


};

}

#endif // _SPELLCORRECTOR_H_
