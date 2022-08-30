#ifndef MOVEMENT_H
# define MOVEMENT_H

#include "initialize_map.h"
#include <iostream>

typedef struct path {
  heap_node_t *hn;
  uint8_t pos[2];
  uint8_t from[2];
  int32_t cost;
  int32_t travel_time;
} path_t;

std::string getMovementInput(int cInput, std::string pcDir, struct Map *map, trainer_t trainers[], int amountOfTrainers, int currentXmap, int currentYmap);
void moveTrainers(trainer_t trainers[], int amountOfTrainers, struct Map *map, int isFirstMove, std::string PCdir, int currentXmap, int currentYmap);
void setDirection(char builtMap[21][80], trainer_t *trainer);

#endif
