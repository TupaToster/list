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

#define userAgreement "I (user) aggree that i fully understand effect that call of this function will take on program's efficiency and still wish to proceed. Therefore I confirm, that no claims about code inefficiency will be made by me."

#ifndef NDEBUG
#define ListDump(list) ListDumpInside (&list, #list, __FILE__, __FUNCTION__, __LINE__) ///< List dump macros
#else
#define ListDump(list) ;
#endif

//End of system defines

struct Nod {

    elem_t value = 0;
    Nod* next    = NULL;
    Nod* prev    = NULL;
};

void NodCtor (Nod* nod);

Nod* NodAddAfter (Nod* nod, elem_t value);

elem_t NodPop (Nod* nod);

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
