#include <stdio.h>
#include "stats.h"

int pvsresearch;
int nullMoveRefutationCount;
int killerMoveRefutationCount;
int alphaBetaCutoffs;

void displayStats() {
        printf("Stats:\n");
        printf("PVS Research: %d\n", pvsresearch);
        printf("Null Move Refutation Count: %d\n", nullMoveRefutationCount);
        printf("Killer Refutation Count: %d\n", killerMoveRefutationCount);
        printf("Alpha-Beta Cutoffs: %d\n", alphaBetaCutoffs);
}
