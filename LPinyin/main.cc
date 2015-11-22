#include "pinyinSyllable.h"
#include "lexicon.h"
#include "syllableGraph.h"
#include "lexiconGraph.h"
#include "languageModel.h"
#include "slmGraph.h"

#include <iostream>

using namespace std;

int main(){
    pinyin::pinyinSyllable ps("syllable.bdt");
    cout << ps.toString();
    pinyin::lexicon lx("lexicon.bdt", ps);
    cout << lx.toString();
    pinyin::syllableGraph sg("tongjiyuyanmoxingpinyinshurufa", ps);
    cout << sg.toString();
    pinyin::lexiconGraph lg(sg, lx, 3);
    cout << lg.toString();
    pinyin::languageModel lm("slm.bdt", lx);
    cout << lm.toString();
    pinyin::slmGraph slmg(lg, lm, 5);
    slmg.MakeSentence();
    return 0;
}
