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
                                " {<prev> Tail = %p | Value = POISON | <next> Head = %p}}\"]\n", list->NullNod->prev, list->NullNod->next);

    Nod* iter = list->NullNod->next;

    for (int i = 1; iter != list->NullNod; i++) {


        picprintf ("\t" "\"Nod_%d\" [shape = \"record\","
                                "style = \"filled\",", i);

        picprintf ("fillcolor = \"#C0FFC0\","
                    "label = \" {Index = %d |"
                    "{ <prev> Prev = %p | Value = " elem_t_F " | <next> Next = %p}} \"]\n", i, iter->prev, iter->value, iter->next);

        iter = iter->next;
    }

    picprintf ("\t" "{rank = same; ");
    for (int i = 0; i <= list->size; i++) {

        picprintf ("Nod_%d; ", i);
    }
    picprintf ("}\n");

    for (int i = 0; i < list->size; i++) {

        picprintf ("\t" "\"Nod_%d\" -> \"Nod_%d\"[style = \"invis\"];\n", i, i + 1);
    }

    for (int i = 0; i <= list->size; i++) {

        if (i != list->size) {

            picprintf ("\t" "\"Nod_%d\":next -> \"Nod_%d\":prev[color = \"blue\"];\n", i, i+1);
        }
        if (i != 0) {

            picprintf ("\t" "\"Nod_%d\":prev -> \"Nod_%d\":next[color = \"red\"];\n", i, i-1);
        }
    }

    picprintf ("}");

    #undef picprintf

    fclose (picSource);

    char command[100] = "";
    sprintf (command, "dot -Tpng %s -o %s", srcName, picName);

    system (command);

    FILE* htm = NULL;
    if (GraphDumpCounter == 1) htm = fopen (HtmLogFile, "w");
    else htm = fopen (HtmLogFile, "a");
    assert (htm != NULL);

    fprintf(htm, "<pre>\n");
    fprintf(htm, "<h2>ListDump on reason %s</h2>\n", why);
    fprintf(htm, "<img src = \"%s\">\n", picName);
    fprintf(htm, "<hr>\n");
    fclose(htm);
}