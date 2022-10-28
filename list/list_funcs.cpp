#include "list.h"

void NodCtor (Nod* nod) {

    if (nod == NULL) return;

    setPoison (nod->value);
    nod->next = 0;
    nod->prev = 0;
}

void ListCtor (List* list) {

    if (list == NULL) return;
    if (isPoison(list->errCode)) ListDtor (list);

    list->canL       = CANL;
    list->size       = 0;
    list->capacity   = 4;
    list->firstEmpty = 1;
    list->errCode    = OK;
    list->List       = (Nod*) calloc ((list->capacity + 1) * sizeof (Nod) + 2 * sizeof (unsigned int), sizeof (char));
    list->ListCanL   = (unsigned int*) list->List;
   *list->ListCanL   = CANL;
    list->List      += sizeof (char);
    list->ListCanR   = (unsigned int*) (list->List + list->capacity + 1);
   *list->ListCanR   = CANR;
    list->canR       = CANR;

    NodCtor (list->List);

    for (int i = 1; i < list->capacity; i++) {

        NodCtor (list->List + i);
        setPoison (list->List[i].value);
        list->List[i].next = (i + 1 == list->capacity) ? 0 : -(i + 1);
    }

    ListCountHash (list);
}

void ListDtor (List* list) {

    if (list == NULL) return;

    setPoison ( list->canL      );
    setPoison ( list->hash      );
    setPoison ( list->size      );
    setPoison ( list->errCode   );
    setPoison (*list->ListCanL  );
    setPoison (*list->ListCanR  );
    setPoison ( list->List      );
    setPoison ( list->canR      );

    for (int i = 0; i < list->capacity + 1;i++) {

        setPoison (list->List[i].value);
        setPoison (list->List[i].prev);
        setPoison (list->List[i].next);
    }

    setPoison (list->capacity);
    free (list->ListCanL);
    setPoison (list->ListCanL);
    setPoison (list->ListCanR);
}

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

unsigned int ListCountHash (List* list) {

    if (list == NULL) return POISON4;

    unsigned int hash = 0;
    unsigned int mult = 1;
    char*        ptr  = (char*) &list->canL;

    for (int i = 0; i < sizeof (unsigned int); i++) {

        hash += mult * (unsigned int) *ptr;
        mult *= MULT;
    }

    for (ptr = (char*) &list->size; ptr != ((char*)&list->canR) + sizeof (unsigned int); ptr++) {

        hash += mult * (unsigned int) *ptr;
        mult *= MULT;
    }

    for (ptr = (char*) list->ListCanL; ptr != ((char*)list->ListCanR) + sizeof (unsigned int); ptr++) {

        hash += mult * (unsigned int) *ptr;
        mult *= MULT;
    }

    list->hash = hash;
    flog (hash);
    return hash;
}

size_t ListErrCheck (List* list) {

    if (list == NULL) return 0;

    //checking for poison
    if (isPoison ( list->errCode  )) {
        list->errCode = POISONED_ERRCODE;
        return list->errCode;
    }
    if (isPoison ( list->canL     ) or
        isPoison ( list->canR     ) or
        isPoison ( list->capacity ) or
        isPoison ( list->size     ) or
        isPoison ( list->List     ) or
        isPoison ( list->ListCanL ) or
        isPoison (*list->ListCanL ) or
        isPoison ( list->ListCanR ) or
        isPoison (*list->ListCanR ) or
        isPoison ( list->hash     )   ) list->errCode |= POISON_ACCESS;
    else {

        for (int i = 0; i < list->capacity;i++) {

            if (isPoison (list->List[i])) {

                list->errCode |= POISON_ACCESS;
                break;
            }
        }
    }
    //endof check

    if ( list->canL     != CANL          ) list->errCode |= BAD_CAN_L;
    if ( list->canR     != CANR          ) list->errCode |= BAD_CAN_R;
    if ( list->ListCanL == NULL          ) list->errCode |= NULL_LIST_CAN_L_PTR;
    if ( list->ListCanR == NULL          ) list->errCode |= NULL_LIST_CAN_R_PTR;
    if (*list->ListCanL != CANL          ) list->errCode |= BAD_LIST_CAN_L;
    if (*list->ListCanR != CANR          ) list->errCode |= BAD_LIST_CAN_R;
    if ( list->List     == NULL          ) list->errCode |= NULL_LIST_PTR;
    if ( list->size     >  list->capacity) list->errCode |= WRONG_SIZE;

    ListCountHash (list);

    return list->errCode;
}

bool ListVerifyHash (List* list) {

    if (list == NULL) return 0;

    if (list->hash != ListCountHash (list)) {

        list->errCode |= WRONG_HASH;
        return 0;
    }

    return 1;
}

void ListDumpInside (List* list, const char* ListName, const char* fileName, const char* funcName, size_t line) {

    if (list == NULL) return;

    ListVerifyHash (list);

    flogprintf ("In file %s, function %s, line %u, list named %s was dumped :" "\n", fileName, funcName, line, ListName);

    flogprintf ("\t" "Errors :" "\n");

    ListLogErrors (list);

                                          flogprintf ("\t" "canL = %u", list->canL);
    if      (isPoison (list->canL))       flogprintf (" (POISONED)" "\n")
    else if (list->canL != CANL   )       flogprintf (" (BAD_CAN)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "hash = %u", list->hash);
    if      (isPoison (list->hash))       flogprintf (" (POISONED)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "size = %u", list->size);
    if      (isPoison (list->size))       flogprintf (" (POISONED)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "capacity = %u", list->capacity);
    if      (isPoison (list->capacity))   flogprintf (" (POISONED)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "firstEmpty = %u", list->firstEmpty);
    if      (isPoison (list->firstEmpty)) flogprintf (" (POISONED)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "errCode = %u", list->errCode);
    if      (isPoison (list->errCode))    flogprintf (" (POISONED)" "\n")
    else                                  flogprintf (" (ok)" "\n")

                                          flogprintf ("\t" "ListCanL = %p", list->ListCanL);
    if      (list->ListCanL == NULL)      flogprintf (" (NULL_PTR)" "\n")
    else {

                                          flogprintf ("\t" "*ListCanL = %u", *list->ListCanL);
        if  (isPoison (*list->ListCanL))  flogprintf (" (POISONED)" "\n")
        else if (*list->ListCanL != CANL) flogprintf (" (BAD_CAN)" "\n")
        else                              flogprintf (" (ok)" "\n")
    }

                                          flogprintf ("\t" "ListCanR = %p", list->ListCanR);
    if      (list->ListCanR == NULL)      flogprintf (" (NULL_PTR)" "\n")
    else {

                                          flogprintf ("\t" "*ListCanR = %u", *list->ListCanR);
        if  (isPoison (*list->ListCanR))  flogprintf (" (POISONED)" "\n")
        else if (*list->ListCanR != CANR) flogprintf (" (BAD_CAN)" "\n")
        else                              flogprintf (" (ok)" "\n")
    }

                                          flogprintf ("\t" "canR = %u", list->canR);
    if      (isPoison (list->canR))       flogprintf (" (POISONED)" "\n")
    else if (list->canR != CANR)          flogprintf (" (BAD_CAN)" "\n")
    else                                  flogprintf (" (ok)" "\n")

    if (isPoison (list->List) or isPoison (list->capacity)) goto Return;

    flogprintf ("\t" "%s.List :" "\n" "\t\t" "Values : ", ListName);
    for (int i = 0; i < list->capacity; i++) {

        if (isPoison (list->List[i].value)) flogprintf ("|    POISON |")
        else flogprintf ("| " elem_t_F " |", list->List[i].value);
    }

    flogprintf ("\n" "\t\t" "Next  :  ");

    for (int i = 0; i < list->capacity; i++) {

        flogprintf ("| %9d |", list->List[i].next);
    }

    flogprintf ("\n" "\t\t" "Prev  :  ");

    for (int i = 0; i < list->capacity; i++) {

        flogprintf ("| %9d |", list->List[i].prev);
    }

    Return:
    flogprintf ("\n" "End of dump" "\n");

    ListCountHash (list);
}

void ListLogErrors (List* list) {

    if (list == NULL) return;

    char names [11][40] = {};
    int iter = 0;

    if (list->errCode & POISON_ACCESS        ) strcpy (names[iter++], "\t\t[POISON_ACCESS       ]\n");
    if (list->errCode & BAD_CAN_L            ) strcpy (names[iter++], "\t\t[BAD_CAN_L           ]\n");
    if (list->errCode & BAD_CAN_R            ) strcpy (names[iter++], "\t\t[BAD_CAN_R           ]\n");
    if (list->errCode & BAD_LIST_CAN_L       ) strcpy (names[iter++], "\t\t[BAD_LIST_CAN_L      ]\n");
    if (list->errCode & BAD_LIST_CAN_R       ) strcpy (names[iter++], "\t\t[BAD_LIST_CAN_R      ]\n");
    if (list->errCode & NULL_LIST_PTR        ) strcpy (names[iter++], "\t\t[NULL_LIST_PTR       ]\n");
    if (list->errCode & NULL_LIST_CAN_L_PTR  ) strcpy (names[iter++], "\t\t[NULL_LIST_CAN_L_PTR ]\n");
    if (list->errCode & NULL_LIST_CAN_R_PTR  ) strcpy (names[iter++], "\t\t[NULL_LIST_CAN_R_PTR ]\n");
    if (list->errCode & WRONG_SIZE           ) strcpy (names[iter++], "\t\t[WRONG_SIZE          ]\n");
    if (list->errCode & POISONED_ERRCODE     ) strcpy (names[iter++], "\t\t[POISONED_ERRCODE    ]\n");
    if (list->errCode & WRONG_HASH           ) strcpy (names[iter++], "\t\t[WRONG_HASH          ]\n");

    if (iter == 0) fprintf (logOutf, "\t\t[ok]\n");
    else
        for (int i = 0; i < iter; i++) fprintf (logOutf, "%s", names[i]);
}

int ListPushElement (List* list, elem_t val, int pos) {

    assert (list != NULL);
    assert (pos > 0);

    //reallocation

    list->size++;
    int currentElem  = list->firstEmpty;
    list->firstEmpty = -list->List[currentElem].next;

    list->List[currentElem].value = val;

    int index = list->List[0].next;

    for (int i = 1; i < pos; i++) {

        if (list->List[index].next == 0) break;

        index = list->List[index].next;
    }

    if (list->List[index].next == 0) {

        list->List[index].next = currentElem;
        list->List[currentElem].prev = index;
        list->List[currentElem].next = 0;
    }
    else {

        list->List[currentElem].next = list->List[index].next;
        list->List[index].next = currentElem;
        list->List[currentElem].prev = index;
    }

    return currentElem;
}

elem_t ListPopElement (List* list, int pos) {

    assert (list != NULL);

    int index = list->List[0].next;

    for (int i = 1; i < pos; i++) {

        if (list->List[index].next == 0) return list->List[0].value;

        index = list->List[index].next;
    }

    list->size--;

    list->List[list->List[index].next].prev = list->List[index].prev;
    list->List[list->List[index].prev].next = list->List[index].next;

    list->List[index].next = -list->firstEmpty;
    list->firstEmpty = index;

    elem_t retVal = list->List[index].value;
    setPoison (list->List[index].value);

    //reallocation

    return retVal;
}