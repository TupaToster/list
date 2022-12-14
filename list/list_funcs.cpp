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
    list->linear     = true;
    list->List       = (Nod*) calloc ((list->capacity + 1) * sizeof (Nod) + 2 * sizeof (unsigned int), sizeof (char));
    assert (list->List != NULL);
    list->ListCanL   = (unsigned int*) list->List;
   *list->ListCanL   = CANL;
    (*(unsigned int**) &list->List) += 1;
    list->ListCanR   = (unsigned int*) (list->List + list->capacity + 1);
   *list->ListCanR   = CANR;
    list->canR       = CANR;

    NodCtor (list->List);

    for (int i = 1; i < list->capacity + 1; i++) {

        NodCtor (list->List + i);
        setPoison (list->List[i].value);
        list->List[i].next = (i == list->capacity) ? 0 : -(i + 1);
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
    return hash;
}

size_t ListErrCheck (List* list) {

    if (list == NULL) return -1;

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
    flogprintf ("| NULL_ELEM |");
    for (int i = 1; i < list->capacity + 1; i++) {

        if (isPoison (list->List[i].value)) flogprintf ("|    POISON |")
        else flogprintf ("| " elem_t_F " |", list->List[i].value)
    }

    flogprintf ("\n" "\t\t" "Next  :  ");

    for (int i = 0; i < list->capacity + 1; i++) {

        flogprintf ("| %9d |", list->List[i].next);
    }

    flogprintf ("\n" "\t\t" "Prev  :  ");

    for (int i = 0; i < list->capacity + 1; i++) {

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

    if (iter == 0) flogprintf ("\t\t[ok]\n")
    else
        for (int i = 0; i < iter; i++) flogprintf ("%s", names[i])
}

int ListInsert (List* list, elem_t val, int prev) {

    assert (list != NULL);
    assert (prev >= 0);

    ListVerifyHash (list);

    ListResize (list);

    if (list->List[prev].next != 0) list->linear = 0;

    list->size ++;

    int currElem = list->firstEmpty;
    list->List[currElem].value = val;
    list->firstEmpty = -list->List[currElem].next;

    list->List[currElem].prev                  = prev;
    list->List[currElem].next                  = list->List[prev].next;
    list->List[prev].next                      = currElem;
    list->List[list->List[currElem].next].prev = currElem;

    ListCountHash (list);

    return currElem;
}

elem_t ListRemoveAndReturn (List* list, int pos) {

    assert (list != NULL);
    assert (pos > 0);
    ListVerifyHash (list);
    list->linear = 0;

    elem_t retVal = list->List[pos].value;
    setPoison (list->List[pos].value);

    list->List[list->List[pos].next].prev = list->List[pos].prev;
    list->List[list->List[pos].prev].next = list->List[pos].next;

    list->List[pos].prev = 0;
    list->List[pos].next = -list->firstEmpty;
    list->firstEmpty = pos;

    ListCountHash (list);

    ListResize (list);

    return retVal;
}

void ListResize (List* list) {

    assert (list != NULL);

    ListVerifyHash (list);

    if (list->firstEmpty == 0) list->capacity *= 2;
    else if (list->size <= list->capacity * 3 / 8 and list->capacity > 4) list->capacity /= 2;
    else return;

    Nod* temp = (Nod*) calloc ((list->capacity + 1) * sizeof (Nod) + 2 * sizeof (unsigned int), sizeof (char));

    assert (temp != NULL);

    void* previousPtr = list->ListCanL;

    *(unsigned int*) temp = *list->ListCanL;
    list->ListCanL = (unsigned int*) temp;

    temp = (Nod*)(list->ListCanL + 1);

    *(unsigned int*) (temp + list->capacity + 1) = *list->ListCanR;
    list->ListCanR = (unsigned int*)(temp + list->capacity + 1);

    temp[0] = list->List[0];
    temp[0].prev = list->size;
    temp[list->size].next = 0;
    for (int i = 1; i <= list->size; i++) {

        temp[i] = list->List[temp[i-1].next];
        temp[i].prev = i-1;
        temp[i-1].next = i;
    }

    list->firstEmpty = list->size + 1;

    for (int i = list->size + 1; i < list->capacity + 1; i++) {

        setPoison (temp[i].value);
        temp[i].next = -(i+1);
    }

    temp[list->capacity].next = 0;
    list->List = temp;

    free (previousPtr);

    list->linear = true;
    ListCountHash (list);
}

int GetRealIndex (List* list, int LogicalNumber, bool DoUReallyWantIt, char* confirmationOfResponsibility) {

    assert (list != NULL);

    if (!DoUReallyWantIt) return -1;
    if (strcmp (confirmationOfResponsibility, "I (user) aggree that i fully understand effect that call of this function will take on program's efficiency and still wish to proceed. Therefore I confirm, that no claims about code inefficiency will be made by me.")) return -1;

    int index = list->List[0].next;

    for (int i = 1; i < LogicalNumber; i++) {

        if (index == 0) break;

        index = list->List[index].next;
    }

    return index;
}

void ListGraphDump (List* list, const char why[], int line) {

    assert (list != NULL);
    assert (why != NULL);

    static int GraphDumpCounter = 0;
    const char HtmLogFile[] = "logs_out.htm";

    GraphDumpCounter++;

    char srcName[] = "GraphDumpSrc.dot";
    char picName[30] = "GraphDumpPic";
    sprintf (picName, "%d.png", GraphDumpCounter);

    FILE* picSource = fopen (srcName, "w");
    assert (picSource != NULL);

    #define picprintf(...) fprintf (picSource, __VA_ARGS__)

    picprintf ("digraph List_%d {" "\n", GraphDumpCounter);
    picprintf ("\t" "graph [dpi = 100];" "\n");
    picprintf ("\t" "splines = ortho" "\n");
    picprintf ("\t" "rankdir = TB" "\n");


    picprintf ("\t" "\"Nod_0\" [shape = \"record\","
                                "style = \"filled\","
                                "fillcolor = \"yellow\","
                                "label = \" {Index = NULL_ELEMENT |"
                                " {<prev> Tail = %d | Value = POISON | <next> Head = %d}}\"]\n", list->List[0].prev, list->List[0].next);

    for (int i = 1; i <= list->capacity; i++) {


        picprintf ("\t" "\"Nod_%d\" [shape = \"record\","
                                "style = \"filled\",", i);

        if (list->List[i].next <= 0 and isPoison (list->List[i].value) and list->List[i].prev == 0) {

            picprintf ("fillcolor = \"red\","
                       "label = \" {Index = %d (FREE_ELEM) |"
                       "{ <prev> Prev = %d | Value = POISON | <next> Next = %d}}\"]\n", i, list->List[i].prev, list->List[i].next);
        }
        else {

            picprintf ("fillcolor = \"green\","
                       "label = \" {Index = %d |"
                       "{ <prev> Prev = %d | Value = " elem_t_F " | <next> Next = %d}} \"]\n", i, list->List[i].prev, list->List[i].value, list->List[i].next);
        }
    }

    picprintf ("\t" "\"firstEmpty\" [shape = \"record\", style = \"filled\", color = \"red\", label = \"firstEmpty = %d\"]\n");

    picprintf ("\t" "{rank = same; ");
    for (int i = 0; i <= list->capacity; i++) {

        picprintf ("Nod_%d; ", i);
    }
    picprintf ("}\n");

    for (int i = 0; i < list->capacity; i++) {

        picprintf ("\t" "\"Nod_%d\" -> \"Nod_%d\"[style = \"invis\"];\n", i, i + 1);
    }

    for (int i = 0; i <= list->capacity; i++) {

        if (abs (list->List[i].next) >= 0 and abs (list->List[i].next) <= list->capacity) {

            picprintf ("\t" "\"Nod_%d\":next -> \"Nod_%d\":prev[color = \"blue\"];\n", i, abs (list->List[i].next));
        }
        if (!(list->List[i].next <= 0 and isPoison (list->List[i].value)) and list->List[i].prev >= 0 and list->List[i].prev <= list->capacity) {

            picprintf ("\t" "\"Nod_%d\":prev -> \"Nod_%d\":next[color = \"red\"];\n", i, list->List[i].prev);
        }
    }

    picprintf ("\t" "\"firstEmpty\" -> \"Nod_%d\";\n}", list->firstEmpty);

    #undef picprintf

    fclose (picSource);

    char command[100] = "";
    sprintf (command, "dot -Tpng %s -o %s", srcName, picName);

    system (command);

    FILE* htm = fopen (HtmLogFile, "a");

    fprintf(htm, "<pre>\n");
    fprintf(htm, "<h2>ListDump on reason %s</h2>\n", why);
    fprintf(htm, "<img src = \"%s\">\n", picName);
    fprintf(htm, "<hr>\n");
    fclose(htm);
}

void ListLinearize (List* list) {

    assert (list != NULL);
    if (list->linear) return;

    ListVerifyHash (list);


    int ind = 0;
    for (int i = 1; i < list->size + 2; i++) {

        int temp = list->List[ind].next;
        list->List[ind].next = i;
        ind = temp;
    }

    ind = list->firstEmpty;

    while (ind != 0) {

        int temp = -list->List[ind].next;
        list->List[ind].next = list->size + 15;
        ind = temp;
    }

    qsort (list->List, list->capacity + 1, sizeof (Nod), ListCmpLin);

    for (int i = 1; i < list->size + 1; i++) {

        list->List[i].prev = i - 1;
    }

    list->List[0].prev = list->size;
    list->List[list->size].next = 0;

    if (list->size == list->capacity) {

        list->firstEmpty = 0;
    }
    else {
        list->firstEmpty = list->size + 1;

        for (int i = list->firstEmpty; i < list->capacity + 1; i++) {

            list->List[i].next = -i - 1;
        }

        list->List[list->capacity].next = 0;
    }

    list->linear = 1;
    ListCountHash (list);
}

int ListCmpLin (const void* a, const void* b) {

    return ( (const Nod*) a)->next - ( (const Nod*) b)->next;
}
