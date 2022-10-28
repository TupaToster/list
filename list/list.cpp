#include "list.h"


int main (int argc, char* argv[]) {

    List lol = {};

    ListCtor (&lol);

    ListDump (lol);

    ListPushElement (&lol, 1, 1);

    ListDump (lol);

    printf ("%Lf", ListPopElement (&lol, 1));

    ListDump (lol);
}