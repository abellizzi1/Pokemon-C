#include <iostream>
#include <string>
#include <curses.h>
#include <limits.h>

#include "movement.h"
#include "heap.h"
#include "draw_windows.h"

#define MAP_X              80
#define MAP_Y              21
#define mapxy(x, y) (m->justTerrainMap[y][x])

int isInMap(int x, int y)
{
    if (x >= 1 && x <= 78 && y >= 1 && y <= 19)
    {
        return 1;
    }
    return 0;
}

static int32_t path_cmp(const void *key, const void *with) {
  return ((path_t *) key)->cost - ((path_t *) with)->cost;
}

static int32_t trainer_cmp(const void *key, const void *with)
{
    return ((trainer_t *) key)->nextMoveTime - ((trainer_t *) with)->nextMoveTime;
}

static void dijkstra_path(struct Map *m, pair_t from, int isHiker, path_t path[21][80])
{
  static path_t *p;
  static uint32_t initialized = 0;
  heap_t h;
  uint32_t x, y;

  if (!initialized) {
    for (y = 0; y < MAP_Y; y++) {
      for (x = 0; x < MAP_X; x++) {
        path[y][x].pos[dim_y] = y;
        path[y][x].pos[dim_x] = x;
      }
    }
    initialized = 1;
  }

  for (y = 0; y < MAP_Y; y++) {
    for (x = 0; x < MAP_X; x++) {
      path[y][x].cost = INT_MAX;

      if (mapxy(x, y) == '.' || mapxy(x, y) == '#')
      {
          path[y][x].travel_time = 10;
      }
      else if (isHiker == 1 && mapxy(x, y) == ':')
      {
          path[y][x].travel_time = 15;
      }
      else if (isHiker == 0 && mapxy(x, y) == ':')
      {
          path[y][x].travel_time = 20;
      }
      else
      {
          path[y][x].travel_time = INT_MAX/2;
      }
    }
  }

  path[from[dim_y]][from[dim_x]].cost = 0;

  heap_init(&h, path_cmp, NULL);

  for (y = 1; y < MAP_Y - 1; y++) {
    for (x = 1; x < MAP_X - 1; x++) {
      path[y][x].hn = heap_insert(&h, &path[y][x]);
    }
  }

  while ((p = (path_t *) heap_remove_min(&h))) {
    p->hn = NULL;

    if ((path[p->pos[dim_y] - 1][p->pos[dim_x]].hn)
        && (path[p->pos[dim_y] - 1][p->pos[dim_x]].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x]].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] - 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x]].hn);
        }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn)
        && (path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] - 1].hn);
        }
    if ((path[p->pos[dim_y]][p->pos[dim_x] - 1].hn)
        && (path[p->pos[dim_y]][p->pos[dim_x] - 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y]][p->pos[dim_x] - 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y]][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] - 1].hn);
        }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn)
        && (path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] - 1].hn);
        }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x]].hn)
        && (path[p->pos[dim_y] + 1][p->pos[dim_x]].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x]].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] + 1][p->pos[dim_x]].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x]].hn);
        }
    if ((path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn)
        && (path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] + 1][p->pos[dim_x] + 1].hn);
        }
    if ((path[p->pos[dim_y]][p->pos[dim_x] + 1].hn)
        && (path[p->pos[dim_y]][p->pos[dim_x] + 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y]][p->pos[dim_x] + 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y]][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y]][p->pos[dim_x] + 1].hn);
        }
    if ((path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn)
        && (path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost >
            (p->cost + p->travel_time)))
        {
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].cost = p->cost + p->travel_time;
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_y] = p->pos[dim_y];
            path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].from[dim_x] = p->pos[dim_x];
            heap_decrease_key_no_replace(&h, path[p->pos[dim_y] - 1][p->pos[dim_x] + 1].hn);
        }
  }
}

int moveTime(char terrain, int isHiker)
{
    // isHiker: 0=Rival, 1=Hiker, 2=PC
    if (terrain == '#' || terrain == '.')
    {
        return 10;
    }
    else if (terrain == ':')
    {
        if (isHiker == 1)
        {
            return 15;
        }
        return 20;
    }
    else if (isHiker == 2 && (terrain == 'M' || terrain == 'C'))
    {
        return 10;
    }
    return 0;
}

void pairFromDirection(std::string dir, pair_t nxt)
{
    if (dir == "ss")
    {
        nxt[dim_y] = nxt[dim_y] + 1;
    }
    else if (dir == "sw")
    {
        nxt[dim_y] = nxt[dim_y] + 1;
        nxt[dim_x] = nxt[dim_x] - 1;
    }
    else if (dir == "ww")
    {
        nxt[dim_x] = nxt[dim_x] - 1;
    }
    else if (dir == "nw")
    {
        nxt[dim_y] = nxt[dim_y] - 1;
        nxt[dim_x] = nxt[dim_x] - 1;
    }
    else if (dir == "nn")
    {
        nxt[dim_y] = nxt[dim_y] - 1;
    }
    else if (dir == "ne")
    {
        nxt[dim_y] = nxt[dim_y] - 1;
        nxt[dim_x] = nxt[dim_x] + 1;
    }
    else if (dir == "ee")
    {
        nxt[dim_x] = nxt[dim_x] + 1;
    }
    else if (dir == "se")
    {
        nxt[dim_y] = nxt[dim_y] + 1;
        nxt[dim_x] = nxt[dim_x] + 1;
    }
}

void oppositeDirection(trainer_t *trainer)
{
    if (trainer->direction == "ss")
    {
        trainer->direction = "nn";
    }
    else if (trainer->direction == "sw")
    {
        trainer->direction = "ne";
    }
    else if (trainer->direction == "ww")
    {
        trainer->direction = "ee";
    }
    else if (trainer->direction == "nw")
    {
        trainer->direction = "se";
    }
    else if (trainer->direction == "nn")
    {
        trainer->direction = "ss";
    }
    else if (trainer->direction == "ne")
    {
        trainer->direction = "sw";
    }
    else if (trainer->direction == "ee")
    {
        trainer->direction = "ww";
    }
    else if (trainer->direction == "se")
    {
        trainer->direction = "nw";
    }
}

void setDirection(char builtMap[21][80], trainer_t *trainer)
{
    int posY = trainer->trainerPos[dim_y];
    int posX = trainer->trainerPos[dim_x];
    if (trainer->type == 'n')
    {
        if (isInMap(posX, posY+1) == 1 && (builtMap[posY+1][posX] == ':' || builtMap[posY+1][posX] == '#' || builtMap[posY+1][posX] == '.'))
        {
            trainer->direction = "ss";
        }
        else if (isInMap(posX-1, posY+1) == 1 && (builtMap[posY+1][posX-1] == ':' || builtMap[posY+1][posX-1] == '#' || builtMap[posY+1][posX-1] == '.'))
        {
            trainer->direction = "sw";
        }
        else if (isInMap(posX-1, posY) == 1 && (builtMap[posY][posX-1] == ':' || builtMap[posY][posX-1] == '#' || builtMap[posY][posX-1] == '.'))
        {
            trainer->direction = "ww";
        }
        else if (isInMap(posX-1, posY-1) == 1 && (builtMap[posY-1][posX-1] == ':' || builtMap[posY-1][posX-1] == '#' || builtMap[posY-1][posX-1] == '.'))
        {
            trainer->direction = "nw";
        }
        else if (isInMap(posX, posY-1) == 1 && (builtMap[posY-1][posX] == ':' || builtMap[posY-1][posX] == '#' || builtMap[posY-1][posX] == '.'))
        {
            trainer->direction = "nn";
        }
        else if (isInMap(posX+1, posY-1) == 1 && (builtMap[posY-1][posX+1] == ':' || builtMap[posY-1][posX+1] == '#' || builtMap[posY-1][posX+1] == '.'))
        {
            trainer->direction = "ne";
        }
        else if (isInMap(posX+1, posY) == 1 && (builtMap[posY][posX+1] == ':' || builtMap[posY][posX+1] == '#' || builtMap[posY][posX+1] == '.'))
        {
            trainer->direction = "ee";
        }
        else if (isInMap(posX+1, posY+1) == 1 && (builtMap[posY+1][posX+1] == ':' || builtMap[posY+1][posX+1] == '#' || builtMap[posY+1][posX+1] == '.'))
        {
            trainer->direction = "se";
        }
    }
    else if (trainer->type == 'p')
    {
        if (isInMap(posX, posY+1) == 1 && builtMap[posY+1][posX] == trainer->initialTerrain)
        {
            trainer->direction = "ss";
        }
        else if (isInMap(posX-1, posY+1) == 1 && builtMap[posY+1][posX-1] == trainer->initialTerrain)
        {
            trainer->direction = "sw";
        }
        else if (isInMap(posX-1, posY) == 1 && builtMap[posY][posX-1] == trainer->initialTerrain)
        {
            trainer->direction = "ww";
        }
        else if (isInMap(posX-1, posY-1) == 1 && builtMap[posY-1][posX-1] == trainer->initialTerrain)
        {
            trainer->direction = "nw";
        }
        else if (isInMap(posX, posY-1) == 1 && builtMap[posY-1][posX] == trainer->initialTerrain)
        {
            trainer->direction = "nn";
        }
        else if (isInMap(posX+1, posY-1) == 1 && builtMap[posY-1][posX+1] == trainer->initialTerrain)
        {
            trainer->direction = "ne";
        }
        else if (isInMap(posX+1, posY) == 1 && builtMap[posY][posX+1] == trainer->initialTerrain)
        {
            trainer->direction = "ee";
        }
        else if (isInMap(posX+1, posY+1) == 1 && builtMap[posY+1][posX+1] == trainer->initialTerrain)
        {
            trainer->direction = "se";
        }
    }
    else if (trainer->type == 'w')
    {
        std::string initDirection = trainer->direction;
        while (initDirection == trainer->direction)
        {
            float directionWanderer = (float) rand() / RAND_MAX;
            if (directionWanderer <= 0.12 && isInMap(posX, posY+1) == 1 && builtMap[posY+1][posX] == trainer->initialTerrain)
            {
                trainer->direction = "ss";
            }
            else if (directionWanderer <= 0.24 && isInMap(posX-1, posY+1) == 1 && builtMap[posY+1][posX-1] == trainer->initialTerrain)
            {
                trainer->direction = "sw";
            }
            else if (directionWanderer <= 0.37 && isInMap(posX-1, posY) == 1 && builtMap[posY][posX-1] == trainer->initialTerrain)
            {
                trainer->direction = "ww";
            }
            else if (directionWanderer <= 0.5 && isInMap(posX-1, posY-1) == 1 && builtMap[posY-1][posX-1] == trainer->initialTerrain)
            {
                trainer->direction = "nw";
            }
            else if (directionWanderer <= 0.62 && isInMap(posX, posY-1) == 1 && builtMap[posY-1][posX] == trainer->initialTerrain)
            {
                trainer->direction = "nn";
            }
            else if (directionWanderer <= 0.74 && isInMap(posX+1, posY-1) == 1 && builtMap[posY-1][posX+1] == trainer->initialTerrain)
            {
                trainer->direction = "ne";
            }
            else if (directionWanderer <= 0.87 && isInMap(posX+1, posY) == 1 && builtMap[posY][posX+1] == trainer->initialTerrain)
            {
                trainer->direction = "ee";
            }
            else if (isInMap(posX+1, posY+1) == 1 && builtMap[posY+1][posX+1] == trainer->initialTerrain)
            {
                trainer->direction = "se";
            }
        }
    }
}

void findNextMoveOther(trainer_t *trainer, char terrainMap[21][80])
{
    pair_t to;
    to[dim_x] = trainer->trainerPos[dim_x];
    to[dim_y] = trainer->trainerPos[dim_y];
    pairFromDirection(trainer->direction, to);

    if (trainer->type == 'p' && ((terrainMap[to[dim_y]][to[dim_x]] != trainer->initialTerrain && terrainMap[to[dim_y]][to[dim_x]] != '@') || (isInMap(to[dim_x], to[dim_y]) == 0)))
    {
        oppositeDirection(trainer);
        to[dim_x] = trainer->trainerPos[dim_x];
        to[dim_y] = trainer->trainerPos[dim_y];
        pairFromDirection(trainer->direction, to);
    }
    else if (trainer->type == 'w' && ((terrainMap[to[dim_y]][to[dim_x]] != trainer->initialTerrain && terrainMap[to[dim_y]][to[dim_x]] != '@') || (isInMap(to[dim_x], to[dim_y]) == 0)))
    {
        setDirection(terrainMap, trainer);
        to[dim_x] = trainer->trainerPos[dim_x];
        to[dim_y] = trainer->trainerPos[dim_y];
        pairFromDirection(trainer->direction, to);
    }
    else if (trainer->type == 'n' && ((isInMap(to[dim_x], to[dim_y]) == 0) || (terrainMap[to[dim_y]][to[dim_x]] != '.' && terrainMap[to[dim_y]][to[dim_x]] != ':'
            && terrainMap[to[dim_y]][to[dim_x]] != '#' && terrainMap[to[dim_y]][to[dim_x]] != '@')))
    {
        oppositeDirection(trainer);
        to[dim_x] = trainer->trainerPos[dim_x];
        to[dim_y] = trainer->trainerPos[dim_y];
        pairFromDirection(trainer->direction, to);
    }
    trainer->trainerNextPos[dim_x] = to[dim_x];
    trainer->trainerNextPos[dim_y] = to[dim_y];
}

int isOccupied(char c)
{
    if (c != 'r' && c != 'h' && c != 'p' && c != 'n' && c != 'w' && c != 's' && c != '"' && c != '%' && c != 'M' && c != 'C')
    {
        return 0;
    }
    return 1;
}

void findNextMoveTrainerRival(pair_t nextMovePair, path_t pathCosts[21][80], pair_t from, char builtMap[21][80])
{
    int currMin = INT_MAX/2;
    if (pathCosts[from[dim_y]-1][from[dim_x]].cost < currMin && isOccupied(builtMap[from[dim_y]-1][from[dim_x]]) == 0)
    {
        currMin = pathCosts[from[dim_y]-1][from[dim_x]].cost;
        nextMovePair[dim_y] = from[dim_y] - 1;
        nextMovePair[dim_x] = from[dim_x];
    }
    if (pathCosts[from[dim_y]-1][from[dim_x]-1].cost < currMin && isOccupied(builtMap[from[dim_y]-1][from[dim_x]-1]) == 0)
    {
        currMin = pathCosts[from[dim_y]-1][from[dim_x]-1].cost;
        nextMovePair[dim_y] = from[dim_y] - 1;
        nextMovePair[dim_x] = from[dim_x] - 1;
    }
    if (pathCosts[from[dim_y]][from[dim_x]-1].cost < currMin && isOccupied(builtMap[from[dim_y]][from[dim_x]-1]) == 0)
    {
        currMin = pathCosts[from[dim_y]][from[dim_x]-1].cost;
        nextMovePair[dim_y] = from[dim_y];
        nextMovePair[dim_x] = from[dim_x] - 1;
    }
    if (pathCosts[from[dim_y]+1][from[dim_x]-1].cost < currMin && isOccupied(builtMap[from[dim_y]+1][from[dim_x]-1]) == 0)
    {
        currMin = pathCosts[from[dim_y]+1][from[dim_x]-1].cost;
        nextMovePair[dim_y] = from[dim_y] + 1;
        nextMovePair[dim_x] = from[dim_x] - 1;
    }
    if (pathCosts[from[dim_y]+1][from[dim_x]].cost < currMin && isOccupied(builtMap[from[dim_y]+1][from[dim_x]]) == 0)
    {
        currMin = pathCosts[from[dim_y]+1][from[dim_x]].cost;
        nextMovePair[dim_y] = from[dim_y] + 1;
        nextMovePair[dim_x] = from[dim_x];
    }
    if (pathCosts[from[dim_y]+1][from[dim_x]+1].cost < currMin && isOccupied(builtMap[from[dim_y]+1][from[dim_x]+1]) == 0)
    {
        currMin = pathCosts[from[dim_y]+1][from[dim_x]+1].cost;
        nextMovePair[dim_y] = from[dim_y] + 1;
        nextMovePair[dim_x] = from[dim_x] + 1;
    }
    if (pathCosts[from[dim_y]][from[dim_x]+1].cost < currMin && isOccupied(builtMap[from[dim_y]][from[dim_x]+1]) == 0)
    {
        currMin = pathCosts[from[dim_y]][from[dim_x]+1].cost;
        nextMovePair[dim_y] = from[dim_y];
        nextMovePair[dim_x] = from[dim_x] + 1;
    }
    if (pathCosts[from[dim_y]-1][from[dim_x]+1].cost < currMin && isOccupied(builtMap[from[dim_y]-1][from[dim_x]+1]) == 0)
    {
        currMin = pathCosts[from[dim_y]-1][from[dim_x]+1].cost;
        nextMovePair[dim_y] = from[dim_y] - 1;
        nextMovePair[dim_x] = from[dim_x] + 1;
    }
}

std::string getMovementInput(int cInput, std::string pcDir, struct Map *map, trainer_t trainers[], int amountOfTrainers, int currentXmap, int currentYmap)
{
    int c;
    int winStartIdx;
    std::string newPcDir = pcDir;
    if (cInput == 'Q')
    {
        newPcDir = "qq";
    }
    else if (cInput == '7' || cInput == 'y')
    {
        newPcDir = "nw";
    }
    else if (cInput == '8' || cInput == 'k')
    {
        newPcDir = "nn";
    }
    else if (cInput == '9' || cInput == 'u')
    {
        newPcDir = "ne";
    }
    else if (cInput == '6' || cInput == 'l')
    {
        newPcDir = "ee";
    }
    else if (cInput == '3' || cInput == 'n')
    {
        newPcDir = "se";
    }
    else if (cInput == '2' || cInput == 'j')
    {
        newPcDir = "ss";
    }
    else if (cInput == '1' || cInput == 'b')
    {
        newPcDir = "sw";
    }
    else if (cInput == '4' || cInput == 'h')
    {
        newPcDir = "ww";
    }
    else if (cInput == 'B')
    {
        WINDOW *bWindow = newwin(20, 29, 3, 26);
        int bagInput;
        bool exitBag = false;
        while (!exitBag)
        {
            mvwprintw(bWindow, 0, 0, "PC's Bag (ESC to Exit)");
            std::string revivesString = "(1) Revives: " + std::to_string(map->trainers[0].numRevives);
            std::string potionsString = "(2) Potions: " + std::to_string(map->trainers[0].numPotions);
            mvwprintw(bWindow, 3, 0, stringToChar(revivesString));
            mvwprintw(bWindow, 4, 0, stringToChar(potionsString));
            wrefresh(bWindow);
            refresh();
            bagInput = getch();
            if (bagInput == 27)
            {
                exitBag = true;
            }
            else if (bagInput == '1') //revive
            {
                if (map->trainers[0].numRevives > 0)
                {
                    WINDOW *reviveWindow = newwin(20, 29, 3, 26);
                    mvwprintw(reviveWindow, 0, 0, "Which Pokemon to revive?");
                    mvwprintw(reviveWindow, 1, 0, "ESC to cancel");
                    for (int i = 0; i < map->trainers[0].numPokemon; i++)
                    {
                        std::string s = "(" + std::to_string(i+1) + ") " + map->trainers[0].pokemonList[i].name;
                        mvwprintw(reviveWindow, (i + 3), 0, stringToChar(s));
                    }
                    wrefresh(reviveWindow);
                    refresh();
                    int reviveInput;
                    bool exitRevive = false;
                    while (!exitRevive)
                    {
                        reviveInput = getch();
                        if (reviveInput == 27)
                        {
                            exitRevive = true;
                        }
                        else if (reviveInput == '1')
                        {
                            if (map->trainers[0].pokemonList[0].hp == 0)
                            {
                                map->trainers[0].pokemonList[0].hp = map->trainers[0].pokemonList[0].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 2 && reviveInput == '2')
                        {
                            if (map->trainers[0].pokemonList[1].hp == 0)
                            {
                                map->trainers[0].pokemonList[1].hp = map->trainers[0].pokemonList[1].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 3 && reviveInput == '3')
                        {
                            if (map->trainers[0].pokemonList[2].hp == 0)
                            {
                                map->trainers[0].pokemonList[2].hp = map->trainers[0].pokemonList[2].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 4 && reviveInput == '4')
                        {
                            if (map->trainers[0].pokemonList[3].hp == 0)
                            {
                                map->trainers[0].pokemonList[3].hp = map->trainers[0].pokemonList[3].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 5 && reviveInput == '5')
                        {
                            if (map->trainers[0].pokemonList[4].hp == 0)
                            {
                                map->trainers[0].pokemonList[4].hp = map->trainers[0].pokemonList[4].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon == 6 && reviveInput == '6')
                        {
                            if (map->trainers[0].pokemonList[5].hp == 0)
                            {
                                map->trainers[0].pokemonList[5].hp = map->trainers[0].pokemonList[5].maxhp/2;
                                map->trainers[0].numRevives = map->trainers[0].numRevives - 1;
                                exitRevive = true;
                            }
                            else
                            {
                                mvwprintw(reviveWindow, 12, 0, "Pokemon cannot be revived");
                                wrefresh(reviveWindow);
                            }
                        }
                    }
                    wclear(reviveWindow);
                    wrefresh(reviveWindow);
                }
                else
                {
                    mvwprintw(bWindow, 18, 0, "Cannot use Revive!");
                    wrefresh(bWindow);
                }
            }
            else if (bagInput == '2') //potion
            {
                if (map->trainers[0].numPotions > 0)
                {
                    WINDOW *potionWindow = newwin(20, 29, 3, 26);
                    mvwprintw(potionWindow, 0, 0, "Which Pokemon to heal?");
                    mvwprintw(potionWindow, 1, 0, "ESC to cancel");
                    for (int i = 0; i < map->trainers[0].numPokemon; i++)
                    {
                        std::string s = "(" + std::to_string(i+1) + ") " + map->trainers[0].pokemonList[i].name;
                        mvwprintw(potionWindow, (i + 3), 0, stringToChar(s));
                    }
                    wrefresh(potionWindow);
                    refresh();
                    int potionInput;
                    bool exitPotion = false;
                    while (!exitPotion)
                    {
                        potionInput = getch();
                        if (potionInput == 27)
                        {
                            exitPotion = true;
                        }
                        else if (potionInput == '1')
                        {
                            if (map->trainers[0].pokemonList[0].hp > 0
                                && map->trainers[0].pokemonList[0].hp < map->trainers[0].pokemonList[0].maxhp)
                            {
                                map->trainers[0].pokemonList[0].hp = map->trainers[0].pokemonList[0].hp + 20;
                                if (map->trainers[0].pokemonList[0].hp >= map->trainers[0].pokemonList[0].maxhp)
                                {
                                    map->trainers[0].pokemonList[0].hp = map->trainers[0].pokemonList[0].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 2 && potionInput == '2')
                        {
                            if (map->trainers[0].pokemonList[1].hp > 0
                                && map->trainers[0].pokemonList[1].hp < map->trainers[0].pokemonList[1].maxhp)
                            {
                                map->trainers[0].pokemonList[1].hp = map->trainers[0].pokemonList[1].hp + 20;
                                if (map->trainers[0].pokemonList[1].hp >= map->trainers[0].pokemonList[1].maxhp)
                                {
                                    map->trainers[0].pokemonList[1].hp = map->trainers[0].pokemonList[1].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 3 && potionInput == '3')
                        {
                            if (map->trainers[0].pokemonList[2].hp > 0
                                && map->trainers[0].pokemonList[2].hp < map->trainers[0].pokemonList[2].maxhp)
                            {
                                map->trainers[0].pokemonList[2].hp = map->trainers[0].pokemonList[2].hp + 20;
                                if (map->trainers[0].pokemonList[2].hp >= map->trainers[0].pokemonList[2].maxhp)
                                {
                                    map->trainers[0].pokemonList[2].hp = map->trainers[0].pokemonList[2].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 4 && potionInput == '4')
                        {
                            if (map->trainers[0].pokemonList[3].hp > 0
                                && map->trainers[0].pokemonList[3].hp < map->trainers[0].pokemonList[3].maxhp)
                            {
                                map->trainers[0].pokemonList[3].hp = map->trainers[0].pokemonList[3].hp + 20;
                                if (map->trainers[0].pokemonList[3].hp >= map->trainers[0].pokemonList[3].maxhp)
                                {
                                    map->trainers[0].pokemonList[3].hp = map->trainers[0].pokemonList[3].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon >= 5 && potionInput == '5')
                        {
                            if (map->trainers[0].pokemonList[4].hp > 0
                                && map->trainers[0].pokemonList[4].hp < map->trainers[0].pokemonList[4].maxhp)
                            {
                                map->trainers[0].pokemonList[4].hp = map->trainers[0].pokemonList[4].hp + 20;
                                if (map->trainers[0].pokemonList[4].hp >= map->trainers[0].pokemonList[4].maxhp)
                                {
                                    map->trainers[0].pokemonList[4].hp = map->trainers[0].pokemonList[4].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                        else if (map->trainers[0].numPokemon == 6 && potionInput == '6')
                        {
                            if (map->trainers[0].pokemonList[5].hp > 0
                                && map->trainers[0].pokemonList[5].hp < map->trainers[0].pokemonList[5].maxhp)
                            {
                                map->trainers[0].pokemonList[5].hp = map->trainers[0].pokemonList[5].hp + 20;
                                if (map->trainers[0].pokemonList[5].hp >= map->trainers[0].pokemonList[5].maxhp)
                                {
                                    map->trainers[0].pokemonList[5].hp = map->trainers[0].pokemonList[5].maxhp;
                                }
                                map->trainers[0].numPotions = map->trainers[0].numPotions - 1;
                                exitPotion = true;
                            }
                            else
                            {
                                mvwprintw(potionWindow, 12, 0, "Pokemon cannot be healed");
                                wrefresh(potionWindow);
                            }
                        }
                    }
                    wclear(potionWindow);
                    wrefresh(potionWindow);
                }
                else
                {
                    mvwprintw(bWindow, 18, 0, "Cannot use Potion!");
                    wrefresh(bWindow);
                }
            }
        }
        wclear(bWindow);
        wrefresh(bWindow);
    }
    else if (cInput == '>')
    {
        if (map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] == 'C' || map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] == 'M')
        {
            std::string pokeBucksStr = "";
            WINDOW *centerMartWin = newwin(21, 80, 1, 0);
            if (map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] == 'C')
            {
                drawCenterWindow(0, map, centerMartWin);
                for (int i = 0; i < map->trainers[0].numPokemon; i++)
                {
                    map->trainers[0].pokemonList[i].hp = map->trainers[0].pokemonList[i].maxhp;
                }
            }
            else
            {
                mvwprintw(centerMartWin, 1, 35, "PokeMart");
                pokeBucksStr = "PokeBucks: " + std::to_string(map->trainers[0].pokeBucks);
                mvwprintw(centerMartWin, 3, 33, stringToChar(pokeBucksStr));
                mvwprintw(centerMartWin, 5, 27, "(1) Buy Potion (100 PokeBucks)");
                mvwprintw(centerMartWin, 6, 27, "(2) Buy Revive (100 PokeBucks)");
                mvwprintw(centerMartWin, 7, 27, "(3) Buy PokeBall (100 PokeBucks");
            }
            mvwprintw(centerMartWin, 1, 34, "Exit with <");
            wrefresh(centerMartWin);
            int c;
            int startIdx = 0;
            while (c != '<')
            {
                c = getch();
                if (map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] == 'M')
                {
                    if (c == '1')
                    {
                        if (map->trainers[0].pokeBucks >= 100)
                        {
                            mvwprintw(centerMartWin, 10, 30, "Successfully bought a Potion!");
                            map->trainers[0].pokeBucks = map->trainers[0].pokeBucks - 100;
                            map->trainers[0].numPotions++;
                        }
                        else
                        {
                            mvwprintw(centerMartWin, 10, 30, "Not Enough PokeBucks!");
                        }
                    }
                    else if (c == '2')
                    {
                        if (map->trainers[0].pokeBucks >= 100)
                        {
                            mvwprintw(centerMartWin, 10, 30, "Successfully bought a Revive!");
                            map->trainers[0].pokeBucks = map->trainers[0].pokeBucks - 100;
                            map->trainers[0].numRevives++;
                        }
                        else
                        {
                            mvwprintw(centerMartWin, 10, 30, "Not Enough PokeBucks!");
                        }
                    }
                    else if (c == '3')
                    {
                        if (map->trainers[0].pokeBucks >= 100)
                        {
                            mvwprintw(centerMartWin, 10, 30, "Successfully bought a PokeBall!");
                            map->trainers[0].pokeBucks = map->trainers[0].pokeBucks - 100;
                            map->trainers[0].numPokeballs++;
                        }
                        else
                        {
                            mvwprintw(centerMartWin, 10, 30, "Not Enough PokeBucks!");
                        }
                    }
                    for (int i = 0; i < 30; i++)
                    {
                        mvwdelch(centerMartWin, 10, 30);
                    }
                    for (int i = 0; i < 20; i++)
                    {
                        mvwdelch(centerMartWin, 3, 33);
                    }
                    pokeBucksStr = "PokeBucks: " + std::to_string(map->trainers[0].pokeBucks);
                    mvwprintw(centerMartWin, 3, 33, stringToChar(pokeBucksStr));
                }
                else
                {
                    if (c == 'a' && map->trainers[0].numPokemon > 1)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[0];
                        map->numPokemonStorage++;
                        for (int i = 0; i < map->trainers[0].numPokemon - 1; i++)
                        {
                            map->trainers[0].pokemonList[i] = map->trainers[0].pokemonList[i+1];
                        }
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == 'b' && map->trainers[0].numPokemon >= 2)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[1];
                        map->numPokemonStorage++;
                        for (int i = 1; i < map->trainers[0].numPokemon - 1; i++)
                        {
                            map->trainers[0].pokemonList[i] = map->trainers[0].pokemonList[i+1];
                        }
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == 'c' && map->trainers[0].numPokemon >= 3)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[2];
                        map->numPokemonStorage++;
                        for (int i = 2; i < map->trainers[0].numPokemon - 1; i++)
                        {
                            map->trainers[0].pokemonList[i] = map->trainers[0].pokemonList[i+1];
                        }
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == 'd' && map->trainers[0].numPokemon >= 4)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[3];
                        map->numPokemonStorage++;
                        for (int i = 3; i < map->trainers[0].numPokemon - 1; i++)
                        {
                            map->trainers[0].pokemonList[i] = map->trainers[0].pokemonList[i+1];
                        }
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == 'e' && map->trainers[0].numPokemon >= 5)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[4];
                        map->numPokemonStorage++;
                        for (int i = 4; i < map->trainers[0].numPokemon - 1; i++)
                        {
                            map->trainers[0].pokemonList[i] = map->trainers[0].pokemonList[i+1];
                        }
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == 'f' && map->trainers[0].numPokemon == 6)
                    {
                        map->pokeStorage[map->numPokemonStorage] = map->trainers[0].pokemonList[5];
                        map->numPokemonStorage++;
                        map->trainers[0].numPokemon--;
                    }
                    else if (c == '1' && map->numPokemonStorage >= 1 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '2' && map->numPokemonStorage >= 2 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+1];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+1; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '3' && map->numPokemonStorage >= 3 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+2];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+2; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '4' && map->numPokemonStorage >= 4 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+3];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+3; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '5' && map->numPokemonStorage >= 5 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+4];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+4; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '6' && map->numPokemonStorage >= 6 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+5];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+5; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '7' && map->numPokemonStorage >= 7 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+6];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+6; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '8' && map->numPokemonStorage >= 8 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+7];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+7; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == '9' && map->numPokemonStorage > 0 && map->trainers[0].numPokemon < 6)
                    {
                        map->trainers[0].pokemonList[map->trainers[0].numPokemon] = map->pokeStorage[startIdx+8];
                        map->numPokemonStorage--;
                        map->trainers[0].numPokemon++;
                        for (int i = startIdx+8; i < map->numPokemonStorage; i++)
                        {
                            map->pokeStorage[i] = map->pokeStorage[i+1];
                        }
                    }
                    else if (c == KEY_UP && startIdx > 0)
                    {
                        startIdx--;
                    }
                    else if (c == KEY_DOWN && startIdx + 13 < map->numPokemonStorage)
                    {
                        startIdx++;
                    }
                    drawCenterWindow(startIdx, map, centerMartWin);
                }
                wrefresh(centerMartWin);
            }
        }
        else
        {
            move(0, 10);
            printw("Not standing on a PokeCenter or PokeMart!");
        }
        newPcDir = "vv";
        drawScreen(map, currentXmap, currentYmap);
        refresh();
    }
    else if (cInput == '5' || cInput == ' ' || cInput == '.')
    {
        newPcDir = "--";
    }
    else if (cInput == 't')
    {
        winStartIdx = 1;
        drawTrainersWindow(winStartIdx, trainers, amountOfTrainers, map->pcPos);
        c = getch();
        while (c != 27)
        {
            /*
            When displaying trainer list, if entire list does not fit in screen and not currently
            at top of list, scroll list up. Opposite for KEY_DOWN
            */
            if (c == KEY_UP && winStartIdx > 1)
            {
                winStartIdx--;
                drawTrainersWindow(winStartIdx, trainers, amountOfTrainers, map->pcPos);
            }
            else if (c == KEY_DOWN && winStartIdx + 18 < amountOfTrainers)
            {
                winStartIdx++;
                drawTrainersWindow(winStartIdx, trainers, amountOfTrainers, map->pcPos);
            }
            c = getch();
        }
        newPcDir = "vv";
        drawScreen(map, currentXmap, currentYmap);
        refresh();
    }
    else
    {
        move(0, 10);
        printw("Key not recognized");
        newPcDir = "vv";
    }
    return newPcDir;
}

void moveTrainers(trainer_t trainers[], int amountOfTrainers, struct Map *map, int isFirstMove, std::string PCdir, int currentXmap, int currentYmap)
{
    static trainer_t *t;
    static path_t path[21][80];
    heap_t h;
    heap_init(&h, trainer_cmp, NULL);
    if (PCdir != "--")
    {
        t = trainers;
        pair_t to;
        to[dim_x] = t[0].trainerPos[dim_x];
        to[dim_y] = t[0].trainerPos[dim_y];
        pairFromDirection(PCdir, to);
        if (map->built_map[to[dim_y]][to[dim_x]] == '%' || map->built_map[to[dim_y]][to[dim_x]] == '"' || map->built_map[to[dim_y]][to[dim_x]] == 'r' || map->built_map[to[dim_y]][to[dim_x]] == 'h')
        {
            if (map->built_map[to[dim_y]][to[dim_x]] == 'r' || map->built_map[to[dim_y]][to[dim_x]] == 'h')
            {
                int immovable = 0;
                for (int i = 1; i < amountOfTrainers; i++)
                {
                    if (trainers[i].reachedPC == 1 && trainers[i].trainerPos[dim_x] == to[dim_x] && trainers[i].trainerPos[dim_y] == to[dim_y])
                    {
                        immovable = 1;
                        break;
                    }
                }
                if (immovable == 1)
                {
                    move(0, 10);
                    printw("You have already defeated this trainer!");
                    while (map->built_map[to[dim_y]][to[dim_x]] == 'r' || map->built_map[to[dim_y]][to[dim_x]] == 'h')
                    {
                        int cInput = getch();
                        PCdir = getMovementInput(cInput, PCdir, map, trainers, amountOfTrainers, currentXmap, currentYmap);
                        to[dim_x] = t[0].trainerPos[dim_x];
                        to[dim_y] = t[0].trainerPos[dim_y];
                        if (PCdir != "vv" && PCdir != "qq")
                        {
                            pairFromDirection(PCdir, to);
                        }
                        else if (PCdir != "qq")
                        {
                            to[dim_x] = t[0].trainerPos[dim_x];
                            to[dim_y] = t[0].trainerPos[dim_y];
                        }
                    }
                }
            }
            else
            {
                move(0, 10);
                printw("There's a tree or boulder in the way!");
                while (map->built_map[to[dim_y]][to[dim_x]] == '%' || map->built_map[to[dim_y]][to[dim_x]] == '"')
                {
                    int cInput = getch();
                    PCdir = getMovementInput(cInput, PCdir, map, trainers, amountOfTrainers, currentXmap, currentYmap);
                    to[dim_x] = t[0].trainerPos[dim_x];
                    to[dim_y] = t[0].trainerPos[dim_y];
                    if (PCdir != "vv" && PCdir != "qq")
                    {
                        pairFromDirection(PCdir, to);
                    }
                    else if (PCdir == "qq")
                    {
                        to[dim_x] = t[0].trainerPos[dim_x];
                        to[dim_y] = t[0].trainerPos[dim_y];
                    }
                }
            }
        }
        t[0].trainerNextPos[dim_x] = to[dim_x];
        t[0].trainerNextPos[dim_y] = to[dim_y];
        t[0].nextMoveTime = moveTime(map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]], 2) + moveTime(map->justTerrainMap[t->trainerNextPos[dim_y]][t->trainerNextPos[dim_x]], 2);
    }
    for (int i = 0; i < amountOfTrainers; i++)
    {
        trainers[i].hn = heap_insert(&h, &trainers[i]);
    }
    while ((t = (trainer_t *)heap_remove_min(&h)))
    {
        t->hn = NULL;
        if (isFirstMove == 0 && (t->reachedPC == 0 || (t->type != 'h' && t->type != 'r')) && t->type != 's')
        {
            map->built_map[t->trainerPos[dim_y]][t->trainerPos[dim_x]] = map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]];
            map->built_map[t->trainerNextPos[dim_y]][t->trainerNextPos[dim_x]] = t->type;
            t->trainerPos[dim_x] = t->trainerNextPos[dim_x];
            t->trainerPos[dim_y] = t->trainerNextPos[dim_y];
            if (t->type == '@')
            {
                map->pcPos[dim_x] = t->trainerNextPos[dim_x];
                map->pcPos[dim_y] = t->trainerNextPos[dim_y];
            }
        }
        else if (t->reachedPC == 1)
        {
            map->built_map[t->trainerPos[dim_y]][t->trainerPos[dim_x]] = t->type;
            map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]] = t->type;
        }
        if (t->type == 'h' && t->reachedPC == 0 && map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] != 'C' && map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] != 'M'
            && map->justTerrainMap[trainers[0].trainerNextPos[dim_y]][trainers[0].trainerNextPos[dim_x]] != 'C' && map->justTerrainMap[trainers[0].trainerNextPos[dim_y]][trainers[0].trainerNextPos[dim_x]] != 'M'
            && map->pcPos[dim_x] != 0 && map->pcPos[dim_y] != 0 && map->pcPos[dim_x] != 79 && map->pcPos[dim_y] != 20 && trainers[0].trainerNextPos[dim_x] != 0
            && trainers[0].trainerNextPos[dim_x] != 79 && trainers[0].trainerNextPos[dim_y] != 0 && trainers[0].trainerNextPos[dim_y] != 20)
        {
            dijkstra_path(map, trainers[0].trainerNextPos, 1, path);
            findNextMoveTrainerRival(t->trainerNextPos, path, t->trainerPos, map->built_map);
            t->nextMoveTime = moveTime(map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]], 1) + moveTime(map->justTerrainMap[t->trainerNextPos[dim_y]][t->trainerNextPos[dim_x]], 1);
        }
        else if (t->type == 'r' && t->reachedPC == 0 && map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] != 'C' && map->justTerrainMap[map->pcPos[dim_y]][map->pcPos[dim_x]] != 'M'
                 && map->justTerrainMap[trainers[0].trainerNextPos[dim_y]][trainers[0].trainerNextPos[dim_x]] != 'C' && map->justTerrainMap[trainers[0].trainerNextPos[dim_y]][trainers[0].trainerNextPos[dim_x]] != 'M'
                 && map->pcPos[dim_x] != 0 && map->pcPos[dim_y] != 0 && map->pcPos[dim_x] != 79 && map->pcPos[dim_y] != 20 && trainers[0].trainerNextPos[dim_x] != 0
                 && trainers[0].trainerNextPos[dim_x] != 79 && trainers[0].trainerNextPos[dim_y] != 0 && trainers[0].trainerNextPos[dim_y] != 20)
        {
            dijkstra_path(map, trainers[0].trainerNextPos, 0, path);
            findNextMoveTrainerRival(t->trainerNextPos, path, t->trainerPos, map->built_map);
            t->nextMoveTime = moveTime(map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]], 1) + moveTime(map->justTerrainMap[t->trainerNextPos[dim_y]][t->trainerNextPos[dim_x]], 0);
        }
        else if (t->type == 'p' || t->type == 'w' || t->type == 'n')
        {
            findNextMoveOther(t, map->built_map);
            t->nextMoveTime = moveTime(map->justTerrainMap[t->trainerPos[dim_y]][t->trainerPos[dim_x]], 1) + moveTime(map->justTerrainMap[t->trainerNextPos[dim_y]][t->trainerNextPos[dim_x]], 0);
        }
        if (isFirstMove == 1)
        {
            t->nextMoveTime = 0;
        }
    }
}
