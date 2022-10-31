#include "list.h"


int main (int argc, char* argv[]) {

    List lol = {};
    ListCtor (&lol);
    ListInsert (&lol, 1, 0);
    ListInsert (&lol, 2, 0);
    ListInsert (&lol, 2, 0);
    ListInsert (&lol, 2, 0);
    ListDump (lol);
    ListResize (&lol);
    ListDump (lol);
}