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

void NodCtor (Nod* nod) {

    setPoison (nod->value);
    nod->next = NULL;
    nod->prev = NULL;
}

Nod* NodAddAfter (Nod* nod, elem_t value) {

    assert (nod != NULL);

    Nod* newNod = (Nod*) calloc (1, sizeof (Nod));
    assert (newNod != NULL);

    NodCtor (newNod);

    newNod->value = value;
    newNod->prev = nod;
    newNod->next = nod->next;

    nod->next = newNod->next;
    newNod->next->prev = newNod;
}

elem_t NodPop (Nod* nod) {

    assert (nod != NULL);

    nod->prev->next = nod->next;
    nod->next->prev = nod->prev;

    elem_t retVal = nod->value;
    NodCtor (nod);
    free (nod);

    return retVal;
}
