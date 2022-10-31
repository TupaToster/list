#include "dumb_list.h"

int main (){

    List lol = {};
    ListCtor (&lol);
    flog ("lol");
    ListDump (&lol);
    flog ("lol");
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    ListAddAfterNod (&lol, lol.NullNod, 1);
    flog ("lol");
    ListDump (&lol);
}