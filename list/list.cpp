#include "list.h"


int main (int argc, char* argv[]) {

    List lol = {};

    ListCtor (&lol);

    ListDump (lol);
    ListPush (&lol, 1, 1);
    ListDump (lol);
    ListPush (&lol, 2, 2);
    ListDump (lol);
    ListPush (&lol, 3, 3);
    ListDump (lol);
    ListPop (&lol, 1);
    ListDump (lol);
}