#include "list.h"


int main (int argc, char* argv[]) {

    List lol = {};
    ListCtor (&lol);
    ListInsert (&lol, 1, 0);
    ListInsert (&lol, 2, 0);
    ListInsert (&lol, 3, 0);
    ListInsert (&lol, 4, 0);
    ListDump (lol);
    ListGraphDump(&lol, "becocs", 1488);
    ListLinearize (&lol);
    ListDump (lol);
    ListGraphDump(&lol, "becocs", 1488);
}