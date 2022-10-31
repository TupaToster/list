#include "dumb_list.h"

void setPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return;

    switch(sizeofVar) {
        case 1 : *((unsigned char*     ) var)      = POISON1; break;
        case 2 : *((unsigned short*    ) var)      = POISON2; break;
        case 4 : *((unsigned int*      ) var)      = POISON4; break;
        case 8 : *((unsigned long long*) var)      = POISON8; break;

        default:
            memset (var, POISON1, sizeofVar);
        break;
    }
}

bool isPoisonInside (void* var, size_t sizeofVar) {

    if (var == NULL) return 1;

    switch(sizeofVar) {
        case 1:
            if ( *( (unsigned char*     ) var) == POISON1) return 1;
            else                                           return 0;
            break;
        case 2:
            if ( *( (unsigned short*    ) var) == POISON2) return 1;
            else                                           return 0;
            break;
        case 4:
            if ( *( (unsigned int*      ) var) == POISON4) return 1;
            else                                           return 0;
            break;
        case 8:
            if ( *( (unsigned long long*) var) == POISON8) return 1;
            else                                           return 0;
            break;
        default:
            for (int i = 0; i < sizeofVar; i++) {

                if (*( ( (unsigned char*) var) + i) != POISON1) return 0;
            }
            return 1;
            break;
    }
}

void NodCtorDtor (Nod* nod) {

    setPoison (nod->value);
    nod->next = NULL;
    nod->prev = NULL;
}

Nod* ListAddAfterNod (List* list, Nod* nod, elem_t val) {

    Nod* newNod = (Nod*) calloc (1, sizeof (Nod));
    NodCtorDtor (newNod);
    list->size++;

    newNod->next    = nod->next;
    newNod->prev    = nod;
    nod->next->prev = newNod;
    nod->next       = newNod;
    newNod->value   = val;

    return newNod;
}

elem_t ListRemoveNod (List* list, Nod* nod) {

    list->size--;
    elem_t retVal = nod->value;
    nod->prev->next = nod->next;
    nod->next->prev = nod->prev;
    NodCtorDtor (nod);
    free (nod);
}

void ListCtor (List* list) {

    list->size = 0;
    list->NullNod = (Nod*) calloc (1, sizeof (Nod));
    NodCtorDtor (list->NullNod);
    list->NullNod->next = list->NullNod;
    list->NullNod->prev = list->NullNod;
}

void ListDump (List* list) {

    flogprintf ("List was dumped somewhere somehow named in an unknown way on a secret line:" "\n");

    flogprintf ("\t" "List :");

    Nod* iter = list->NullNod->next;

    flogprintf ("\n" "\t\t" "Adress : ");

    flogprintf ("| %9p |", list->NullNod);

    iter = list->NullNod->next;

    while (iter != list->NullNod and iter != NULL) {

        flogprintf ("| %9p |", iter);
        iter = iter->next;
    }

    flogprintf ("\n" "\t\t" "Value  : ");

    iter = list->NullNod->next;

    flogprintf ("| NULL_ELEM |");

    while (iter != list->NullNod and iter != NULL) {

        if (isPoison (iter->value)) flogprintf ("|    POISON |")
        else flogprintf ("| " elem_t_F " |", iter->value)
        iter = iter->next;
    }

    flogprintf ("\n" "\t\t" "Next   : ");

    flogprintf ("| %9p |", list->NullNod->next);
    iter = list->NullNod->next;

    while (iter != list->NullNod and iter != NULL) {

        flogprintf ("| %9p |", iter->next);
        iter = iter->next;
    }

    flogprintf ("\n" "\t\t" "Prev   : ");

    flogprintf ("| %9p |", list->NullNod->prev);

    iter = list->NullNod->next;

    while (iter != list->NullNod and iter != NULL) {

        flogprintf ("| %9p |", iter->prev);
        iter = iter->next;
    }

    flogprintf ("\n" "End of dump" "\n");

}