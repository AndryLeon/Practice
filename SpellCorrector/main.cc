#include "SpellCorrector.h"

#include <iostream>
#include <string>

using namespace std;

int main(){
    spell::SpellCorrector spell;
    spell.load("big.txt");
    cout << spell.correct("acess") << "\n";
    return 0;
}
