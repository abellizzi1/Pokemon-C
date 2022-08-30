#ifndef DRAW_WINDOWS_H
# define DRAW_WINDOWS_H

#include "initialize_map.h"

void drawTrainersWindow(int startIdx, trainer_t trainers[], int amountOfTrainers, pair_t pcPos);
void drawCenterWindow(int startIdx, struct Map *map, WINDOW *centerMartWin);
void drawScreen(struct Map *map, int currentXmap, int currentYmap);

#endif
