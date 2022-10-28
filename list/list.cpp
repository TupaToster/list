#include "list.h"


int main (int argc, char* argv[]) {

    List lol = {};
    
    ListCtor (&lol);

    ListPush (&lol, 1, 0);
    ListPush (&lol, 1, 0);
    ListPush (&lol, 1, 0);
    ListPush (&lol, 1, 0);
    ListPush (&lol, 1, 0);
    ListPush (&lol, 1, 0);
    ListDump (lol);
}