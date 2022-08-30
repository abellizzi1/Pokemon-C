#ifndef INITIALIZE_MAP_H
# define INITIALIZE_MAP_H

#include "file_parsing.h"
#include "heap.h"

typedef enum dim {
  dim_x,
  dim_y,
  num_dims
} dim_t;
typedef uint8_t pair_t[num_dims];

typedef class
{
public:
    std::string name;
    int level;
    std::string gender;
    bool isShiny;
    Moves moveOne;
    Moves moveTwo;
    int hp;
    int maxhp;
    int attack;
    int defense;
    int specAttack;
    int specDefense;
    int speed;
    int baseSpeed;
    int numMoves;
    int typeOne;
    int typeTwo;
} pokemon_class;

typedef class
{
public:
    heap_node_t *hn;
    pair_t trainerPos;
    pair_t trainerNextPos;
    char type;
    int32_t nextMoveTime;
    std::string direction; //nn, ss, ee, ww, ne, se, sw, nw
    char initialTerrain;
    int reachedPC;
    int id;
    int numPokemon;
    int numPotions;
    int numRevives;
    int numPokeballs;
    int pokeBucks;
    pokemon_class *pokemonList;
} trainer_t;

class Map
{
public:
    char built_map[21][80];
    char justTerrainMap[21][80];
    int top_exit;
    int bottom_exit;
    int left_exit;
    int right_exit;
    int maxPokemonLevel;
    int minPokemonLevel;
    int numPokemonStorage;
    pair_t pcPos;
    pokemon_class *pokeStorage;
    trainer_t trainers[];
};

void drawScreen(struct Map *map, int currentXmap, int currentYmap);
void copy2dArrayToPointer(char map[21][80], struct Map *allMapsCurr[399][399], int currentXmap, int currentYmap);
char *stringToChar(std::string s);
void generateMap(char map[21][80], struct Map *allMapsCurr[399][399], int currentXmap, int currentYmap);

#endif
