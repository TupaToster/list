#include "dumb_list.h"

int main (){

    List lol = {};
    ListCtor (&lol);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListGraphDump (&lol, "becocs", 228);
}