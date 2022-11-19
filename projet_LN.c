#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_STEP 20
#define NB_CONCEPT 50
#define NB_WORD 1000
#define MAX_SIZE_WORD 15

int main(void) {
    // variables
    int NJ, J;

    char* list_word[NB_WORD][NB_CONCEPT+1];
    char list_concept[NB_CONCEPT][255];
    int counter_word = 0;
    int counter_concept = 0;

    // Initialisation phase:
    char line[255]; // to optimize

    memset(line, 0, 255);
    scanf("%[^\n]s", line);
    fprintf(stderr, "%s\n", line);
    fflush(stderr);
    // First line
    sscanf(line, "%d %d", &NJ, &J);
    // fprintf(stderr, "Recu : %s\n", *line);
    fprintf(stderr, "Nombre de joueur : %d; Mon numéro de pythie : %d\n", NJ, J);
    fflush(stderr);


    for (int i = 0; i < NB_CONCEPT; i++) {
        memset(line, 0, 255);
        scanf("%[^\n]s", line);

        sscanf(line, "%s", list_concept[counter_concept]);
        fprintf(stderr, "concept numéro %d : %s\n", counter_concept+1, line);
        fprintf(stderr, "concept numéro %d : %s\n", counter_concept+1, list_concept[counter_concept]);
        fflush(stderr);
        counter_concept++;
    }

    for (int i = 0; i < NB_WORD; i++) {
        memset(line, 0, 255);
        scanf("%[^\n]s", line);
        // à optimiser
        sscanf(line, "%s", list_word[counter_word][0]);
        counter_word++;
    }


    // every row
    for (int i = 0; i < NB_STEP; i++) {
        printf("PASS\n");
    }

    return 0;

}