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

#ifndef NDEBUG
#define ListDump(list) ListDumpInside (&list, #list, __FILE__, __FUNCTION__, __LINE__) ///< Stack dump macros
#else
#define ListDump(list) ;
#endif

//End of system defines

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

struct Nod {

    elem_t value = 0;
    int next = 0;
    int prev = 0;
};

struct List {

    unsigned int  canL       = CANL;
    unsigned int  hash       = 0;
    size_t        size       = 0;
    size_t        capacity   = 0;
    int           firstEmpty = 0;
    size_t        errCode    = OK;
    unsigned int* ListCanL   = NULL;
    Nod*          List       = NULL;
    unsigned int* ListCanR   = NULL;
    unsigned int  canR       = CANR;
};

void NodCtor (Nod* nod);

void ListCtor (List* list);

void ListDtor (List* list);

void setPoisonInside (void* var, size_t sizeofVar);

bool isPoisonInside (void* var, size_t sizeofVar);

unsigned int ListCountHash (List* list);

size_t ListErrCheck (List* list);

bool ListVerifyHash (List* list);

void ListDumpInside (List* list, const char* ListName, const char* fileName, const char* funcName, size_t line);

void ListLogErrors (List* list);

int ListPushElement (List* list, elem_t val, int pos);

elem_t ListPopElement (List* list, int pos);
