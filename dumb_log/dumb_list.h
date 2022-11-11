#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "..\lib\globalization.h"
#include header(flog)

typedef double elem_t;
#define elem_t_F "%9Lf"
// End of user defines

//System defines

#define CANL 0xDEADBEEF ///< Left cannary of a structure
#define CANR 0xD34DB33F ///< Right cannary of a structure

#define POISON1 0xBD ///< 1 byte Poison
#define POISON2 0xBDCF ///< 2 byte Poison
#define POISON4 0xBADC0FEE ///< 4 byte Poison
#define POISON8 0xBADC0FEEF04DED32 ///< 8 byte Poison

#define MULT 37u ///< Multiplier for hash

#define setPoison(var) setPoisonInside (&var, sizeof (var)) ///< Macros for auto-poison

#define isPoison(var) isPoisonInside (&var, sizeof (var)) ///< Macros for checking for poison

//End of system defines

struct Nod {

    elem_t value = 0;
    Nod* next    = NULL;
    Nod* prev    = NULL;
};

struct List {

    size_t size   = 0;
    Nod* NullNod  = NULL;
};

void ListCtor (List* list);

void NodCtorDtor (Nod* nod);

Nod* ListAddAfterNod (List* list, Nod* nod, elem_t val);

elem_t ListRemoveNod (List* list, Nod* nod);

/// @brief Sets a variable to a Poison value
/// @param var Var to set
/// @param sizeofVar Size of the variable
void setPoisonInside (void* var, size_t sizeofVar);

/// @brief Checks if the var is poisoned
/// @param var Var to check
/// @param sizeofVar Size of variable
/// @return true - if is poisoned
/// @return false - if not
bool isPoisonInside (void* var, size_t sizeofVar);

void ListDump (List* list);

void ListGraphDump (List* list, const char why[], int line);
