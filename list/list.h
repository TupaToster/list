#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "..\lib\globalization.h"
#include header(flog)

// User defines
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

/// @brief Error codes for list
enum ListErrorCodes {
    OK                   = 0,    ///< All ok
    POISON_ACCESS        = 1<<0, ///< One or more struct elements is poison
    BAD_CAN_L            = 1<<1, ///< Dead left cannary of structure
    BAD_CAN_R            = 1<<2, ///< Dead right cannary of structure
    BAD_LIST_CAN_L       = 1<<3, ///< Dead left cannary of stack
    BAD_LIST_CAN_R       = 1<<4, ///< Dead right cannary of stack
    NULL_LIST_PTR        = 1<<5, ///< NULL ptr for stack
    NULL_LIST_CAN_L_PTR  = 1<<6, ///< NULL ptr for left stack cannary
    NULL_LIST_CAN_R_PTR  = 1<<7, ///< NULL ptr for right stack cannary
    WRONG_SIZE           = 1<<8, ///< Size is more than capacity
    POISONED_ERRCODE     = 1<<9, ///< ErrCode variable is poisoned; Ususally means that struct has been destructed
    WRONG_HASH           = 1<<10 ///< Hash was changed without any changes from specified function
};

/// @brief An element of list
struct Nod {

    elem_t value = 0; ///<Value of element
    int    next = 0;  ///<Index of next elem
    int    prev = 0;  ///<Index of previous elem
};

/// @brief List structure with effective memory usage and speed
struct List {

    unsigned int  canL       = CANL;  ///<Left cannary of struct
    unsigned int  hash       = 0;     ///<Hash for structure
    size_t        size       = 0;     ///<Size of list
    size_t        capacity   = 0;     ///<Capacity of list (without zero element)
    int           firstEmpty = 0;     ///<First Empty element of list
    size_t        errCode    = OK;    ///<Error code
    unsigned int* ListCanL   = NULL;  ///<Left cannary of list
    Nod*          List       = NULL;  ///<List itself
    unsigned int* ListCanR   = NULL;  ///<Right cannary of list
    unsigned int  canR       = CANR;  ///<Right cannary of struct
};

/// @brief Constructs a Nod struct
/// @param nod Ptr to Nod to construct
void NodCtor (Nod* nod);

/// @brief Constructs a List struct
/// @param list Ptr to List to construct
void ListCtor (List* list);

/// @brief Destructs a List struct
/// @param list Ptr to List struct to destroy
void ListDtor (List* list);

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

/// @brief Counts hash for a list
/// @param list Ptr to List var to count for
/// @return Counted hash, or POISON4 if list == NULL
unsigned int ListCountHash (List* list);

/// @brief Checks for errors in list
/// @param list Ptr to List var to check
/// @return Error code for list, -1 if list == NULL
size_t ListErrCheck (List* list);

/// @brief Verifies hash to be unchanged amongst operations and writes a correct one
/// @param list Ptr to List var to verify
/// @return true if hash is correct, false otherwise
bool ListVerifyHash (List* list);

/// @brief Dumps a List
/// @param list Ptr to var to dump
/// @param ListName Name of List
/// @param fileName Name of file it was dumped at
/// @param funcName Name of function it was dumped at
/// @param line Line of file
void ListDumpInside (List* list, const char* ListName, const char* fileName, const char* funcName, size_t line);

/// @brief Logs errors in a special way
/// @param list Ptr to List to log
void ListLogErrors (List* list);

/// @brief Pushes an element on position pos or to the end of List if pos > size
/// @param list List to push to
/// @param val Value to push
/// @param pos Position to push to
/// @return Index in List->List of a pushed element
int ListPush (List* list, elem_t val, int pos);

/// @brief Pops and element on position pos
/// @param list List to pop from
/// @param pos Position to pop
/// @return Popped element
elem_t ListPop (List* list, int pos);

/// @brief Resizes List if it's time to do it
/// @param list List to resize
void ListResize (List* list);

/// @brief Finds an element by logical number
/// @param list Ptr to list variable
/// @param LogicalNumber Number of elem in list
/// @param step Step to move ptr for
/// @param DoUReallyWantIt Do u really want it? (true/false)
/// @param confirmationOfResponsibility Look up userAgreement
/// @return index of an element or -1 if u failed as a human
int GetRealIndex (List* list, int LogicalNumber, int step, bool DoUReallyWantIt, char* confirmationOfResponsibility);