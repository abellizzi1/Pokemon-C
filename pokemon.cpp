#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include <unistd.h>
#include <curses.h>
#include "heap.h"
#include "file_parsing.h"
#include "initialize_map.h"
#include "movement.h"
#include "draw_windows.h"

void spawnTrainer(char builtMap[21][80], trainer_t *trainer)
{
    int randXtrainer = (rand() % 78) + 1;
    float randStartBottomOrTop = (float) rand() / RAND_MAX;
    int randYtrainer = (rand() % 4) + 2;
    if (randStartBottomOrTop <= 0.5)
    {
        for (int y = randYtrainer+1; y < 20; y++)
        {
            if (builtMap[y][randXtrainer] == ':' || builtMap[y][randXtrainer] == '#' || builtMap[y][randXtrainer] == '.')
            {
                trainer->trainerPos[dim_x] = randXtrainer;
                trainer->trainerPos[dim_y] = y;
                break;
            }
        }
    }
    else
    {
        for (int y = 20-randYtrainer; y >= 1; y--)
        {
            if (builtMap[y][randXtrainer] == ':' || builtMap[y][randXtrainer] == '#' || builtMap[y][randXtrainer] == '.')
            {
                trainer->trainerPos[dim_x] = randXtrainer;
                trainer->trainerPos[dim_y] = y;
                break;
            }
        }
    }
    if (trainer->type == 'w' || trainer->type == 'p')
    {
        trainer->initialTerrain = builtMap[trainer->trainerPos[dim_y]][trainer->trainerPos[dim_x]];
    }
    builtMap[trainer->trainerPos[dim_y]][trainer->trainerPos[dim_x]] = trainer->type;
}

bool checkIfNumber(std::string s)
{
    for (int i = 0; i < (int)s.length(); i++)
    {
        if (!std::isdigit(s.at(i)))
        {
            return false;
        }
    }
    return true;
}

void generatePokemon(pokemon_class *newPokemon, AllPokemonArrays *allArrays, struct Map *map)
{
    int randPokemon = rand() % 1092;
    newPokemon->name = allArrays->pokemonList[randPokemon].identifier;
    newPokemon->level = (rand() % (map->maxPokemonLevel - map->minPokemonLevel)) + map->minPokemonLevel;
    newPokemon->numMoves = 0;
    int halfChance = rand() % 2;
    if (halfChance == 0)
    {
        newPokemon->gender = "Male";
    }
    else
    {
        newPokemon->gender = "Female";
    }
    int shinyChance = rand() % 8192;
    if (shinyChance == 0)
    {
        newPokemon->isShiny = true;
    }
    else
    {
        newPokemon->isShiny = false;
    }
    int numMoves = 0;
    for (int i = 0; i < allArrays->pokemonMovesListLength; i++)
    {
        if (allArrays->pokemonList[randPokemon].species_id == allArrays->pokemonMovesList[i].pokemon_id && allArrays->pokemonMovesList[i].pokemon_move_method_id == 1
            && allArrays->pokemonMovesList[i].level <= newPokemon->level)
        {
            numMoves++;
        }
    }
    if (numMoves > 0)
    {
        int randMoveOne = rand() % numMoves;
        int randMoveTwo;
        int moveOneId;
        int moveTwoId;
        if (randMoveOne > 0 && numMoves > 1)
        {
            randMoveTwo = randMoveOne - 1;
        }
        else if (numMoves > 1)
        {
            randMoveTwo = randMoveOne + 1;
        }
        numMoves = 0;
        for (int i = 0; i < allArrays->pokemonMovesListLength; i++)
        {
            if (allArrays->pokemonList[randPokemon].species_id == allArrays->pokemonMovesList[i].pokemon_id && allArrays->pokemonMovesList[i].pokemon_move_method_id == 1
            && allArrays->pokemonMovesList[i].level <= newPokemon->level)
            {
                if (randMoveOne == numMoves)
                {
                    moveOneId = allArrays->pokemonMovesList[i].move_id;
                }
                else if (randMoveTwo == numMoves)
                {
                    moveTwoId = allArrays->pokemonMovesList[i].move_id;
                }
                numMoves++;
            }
        }
        for (int i = 0; i < allArrays->movesListLength; i++)
        {
            if (allArrays->movesList[i].id == moveOneId)
            {
                newPokemon->moveOne = allArrays->movesList[i];
                newPokemon->numMoves = newPokemon->numMoves + 1;
            }
            else if (allArrays->movesList[i].id == moveTwoId)
            {
                newPokemon->moveTwo = allArrays->movesList[i];
                newPokemon->numMoves = newPokemon->numMoves + 1;
            }
        }
    }
    newPokemon->hp = rand() % 16;
    newPokemon->attack = rand() % 16;
    newPokemon->defense = rand() % 16;
    newPokemon->specAttack = rand() % 16;
    newPokemon->specDefense = rand() % 16;
    newPokemon->speed = rand() % 16;
    for (int i = 0; i < allArrays->pokemonStatsListLength; i++)
    {
        if (allArrays->pokemonStatsList[i].pokemon_id == allArrays->pokemonList[randPokemon].species_id)
        {
            switch (allArrays->pokemonStatsList[i].stat_id)
            {
            case (1):
                newPokemon->hp = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->hp) * 2) * newPokemon->level)/100) + newPokemon->level + 10;
                break;
            case (2):
                newPokemon->attack = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->attack) * 2) * newPokemon->level)/100) + 5;
                break;
            case (3):
                newPokemon->defense = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->defense) * 2) * newPokemon->level)/100) + 5;
                break;
            case (4):
                newPokemon->specAttack = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->specAttack) * 2) * newPokemon->level)/100) + 5;
                break;
            case (5):
                newPokemon->specDefense = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->specDefense) * 2) * newPokemon->level)/100) + 5;
                break;
            default:
                newPokemon->speed = ((((allArrays->pokemonStatsList[i].base_stat + newPokemon->speed) * 2) * newPokemon->level)/100) + 5;
                newPokemon->baseSpeed = allArrays->pokemonStatsList[i].base_stat;
                break;
            }
        }
    }
    newPokemon->maxhp = newPokemon->hp;
    bool isDualType = false;
    newPokemon->typeOne = -1;
    newPokemon->typeTwo = -1;

    for (int i = 0; i < allArrays->pokemonTypesListLength; i++)
    {
        if (allArrays->pokemonTypesList[i].pokemon_id == randPokemon)
        {
            if (!isDualType)
            {
                newPokemon->typeOne = allArrays->pokemonTypesList[i].type_id;
                isDualType = true;
            }
            else
            {
                newPokemon->typeTwo = allArrays->pokemonTypesList[i].type_id;
            }
        }
    }
}

void initializeTrainersHelper(struct Map *map, char trainerType, int idxAllTrainers, AllPokemonArrays *allArrays)
{
    map->trainers[idxAllTrainers].type = trainerType;
    map->trainers[idxAllTrainers].nextMoveTime = 0;
    map->trainers[idxAllTrainers].reachedPC = 0;
    map->trainers[idxAllTrainers].numPokemon = 0;
    if (trainerType != '@')
    {
        map->trainers[idxAllTrainers].numPokemon = 1;
        float frandom = (float) rand() / RAND_MAX;
        if (frandom <= 0.08)
        {
            map->trainers[idxAllTrainers].numPokemon = 6;
        }
        else if (frandom <= 0.13)
        {
            map->trainers[idxAllTrainers].numPokemon = 5;
        }
        else if (frandom <= 0.22)
        {
            map->trainers[idxAllTrainers].numPokemon = 4;
        }
        else if (frandom <= 0.36)
        {
            map->trainers[idxAllTrainers].numPokemon = 3;
        }
        else if (frandom <= 0.6)
        {
            map->trainers[idxAllTrainers].numPokemon = 2;
        }
        map->trainers[idxAllTrainers].pokemonList = new pokemon_class[map->trainers[idxAllTrainers].numPokemon]();
        for (int i = 0; i < map->trainers[idxAllTrainers].numPokemon; i++)
        {
            generatePokemon(&map->trainers[idxAllTrainers].pokemonList[i], allArrays, map);
        }
    }
}

void initializeTrainers(struct Map *allMaps[399][399], int currentXmap, int currentYmap, int numTrainers, AllPokemonArrays *allArrays)
{
    int idxAllTrainers = 0;
    initializeTrainersHelper(allMaps[currentXmap][currentYmap], '@', idxAllTrainers, allArrays);
    allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers].trainerPos[dim_x] = allMaps[currentXmap][currentYmap]->pcPos[dim_x];
    allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers].trainerPos[dim_y] = allMaps[currentXmap][currentYmap]->pcPos[dim_y];
    allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers].trainerNextPos[dim_y] = allMaps[currentXmap][currentYmap]->pcPos[dim_y];
    allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers].trainerNextPos[dim_x] = allMaps[currentXmap][currentYmap]->pcPos[dim_x];
    idxAllTrainers++;
    if (numTrainers == 1)
    {
        initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'r', idxAllTrainers, allArrays);
        spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
    }
    else if (numTrainers > 1)
    {
        initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'h', idxAllTrainers, allArrays);
        spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
        idxAllTrainers++;

        initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'r', idxAllTrainers, allArrays);
        spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
        idxAllTrainers++;
        float chanceTrainer = (float) rand() / RAND_MAX;
        for (int i = 0; i < numTrainers-2; i++)
        {
            if (chanceTrainer <= 0.1)
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'r', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            else if (chanceTrainer <= 0.2)
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'h', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            else if (chanceTrainer <= 0.4)
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'p', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                setDirection(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            else if (chanceTrainer <= 0.6)
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'w', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                setDirection(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            else if (chanceTrainer <= 0.8)
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 's', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            else
            {
                initializeTrainersHelper(allMaps[currentXmap][currentYmap], 'n', idxAllTrainers, allArrays);
                spawnTrainer(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                setDirection(allMaps[currentXmap][currentYmap]->built_map, &allMaps[currentXmap][currentYmap]->trainers[idxAllTrainers]);
                idxAllTrainers++;
            }
            chanceTrainer = (float) rand() / RAND_MAX;
        }
    }
    for (int i = 0; i < numTrainers+1; i++)
    {
        allMaps[currentXmap][currentYmap]->trainers[i].id = i;
    }
}

void printPokemonStats(WINDOW *win, int startRow, int startCol, pokemon_class *poke)
{
    std::string pokemonNameString = "Name: " + poke->name;
    mvwprintw(win, startRow, startCol, stringToChar(pokemonNameString));
    std::string levelString = "Level: " + std::to_string(poke->level);
    mvwprintw(win, startRow + 1, startCol, stringToChar(levelString));
    std::string genderString = "Gender: " + poke->gender;
    mvwprintw(win, startRow + 2, startCol, stringToChar(genderString));
    std::string isShinyString = "Shiny: ";
    if (poke->isShiny)
    {
        isShinyString = isShinyString + "Yes";
    }
    else
    {
        isShinyString = isShinyString + "No";
    }
    mvwprintw(win, startRow + 3, startCol, stringToChar(isShinyString));
    std::string moveOneString = "Move 1: " + poke->moveOne.identifier;
    std::string moveTwoString = "Move 2: " + poke->moveTwo.identifier;
    mvwprintw(win, startRow + 5, startCol, stringToChar(moveOneString));
    mvwprintw(win, startRow + 6, startCol, stringToChar(moveTwoString));
    std::string hpString = "HP: " + std::to_string(poke->hp);
    mvwprintw(win, startRow + 7, startCol, stringToChar(hpString));
    std::string attackString = "Attack: " + std::to_string(poke->attack);
    mvwprintw(win, startRow + 8, startCol, stringToChar(attackString));
    std::string defenseString = "Defense: " + std::to_string(poke->defense);
    mvwprintw(win, startRow + 9, startCol, stringToChar(defenseString));
    std::string specAttackString = "Special Attack: " + std::to_string(poke->specAttack);
    mvwprintw(win, startRow + 10, startCol, stringToChar(specAttackString));
    std::string specDefenseString = "Special Defense: " + std::to_string(poke->specDefense);
    mvwprintw(win, startRow + 11, startCol, stringToChar(specDefenseString));
    std::string speedString = "Speed: " + std::to_string(poke->speed);
    mvwprintw(win, startRow + 12, startCol, stringToChar(speedString));
}

bool isAllKnockedOut(struct Map *map, int trainerIdx)
{
    for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
    {
        if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
        {
            return false;
        }
    }
    return true;
}

float typeEffective(int moveType, pokemon_class *target)
{
    /*
        Pokemon types table (rows and columns in same order):
        1 Normal
        2 Fighting
        3 Flying
        4 Poison
        5 Ground
        6 Rock
        7 Bug
        8 Ghost
        9 Steel
        10 Fire
        11 Water
        12 Grass
        13 Electric
        14 Psychic
        15 Ice
        16 Dragon
        17 Dark
        18 Fairy
    */
    float ret = 1;
    float typeTable[18][18] = {
        {1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5},
        {1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1},
        {1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2},
        {1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1},
        {1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1},
        {1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5},
        {0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1},
        {1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2},
        {1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1},
        {1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1},
        {1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1},
        {1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1},
        {1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1},
        {1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0},
        {1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5},
        {1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1}
    };

    for (int i = 0; i < 18; i++)
    {
        if ((target->typeOne - 1) == i)
        {
            ret *= typeTable[moveType-1][target->typeOne - 1];
        }
        else if ((target->typeTwo - 1) == i)
        {
            ret *= typeTable[moveType-1][target->typeTwo - 1];
        }
    }
    return ret;
}

void pokemonBattle(struct Map *map, AllPokemonArrays *allArrays, bool isTrainerBattle, int trainerIdx)
{
    pokemon_class wildPokemon;
    generatePokemon(&wildPokemon, allArrays, map);
    WINDOW *pokeWindow = newwin(21, 80, 1, 0);
    int c;
    bool escape = false;
    pokemon_class *currentPokemon = &map->trainers[0].pokemonList[0];
    pokemon_class *trainerCurrentPokemon;
    std::string pokeWindowMessage1 = "";
    std::string pokeWindowMessage2 = "";
    Moves idleMove;
    int idleMoveDamage;
    bool isIdleMove = false;
    int numRunAttempts = 0;
    if (isTrainerBattle)
    {
        trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[0];
    }
    while (!escape)
    {
        if (isTrainerBattle)
        {
            float attackRandom = ((float)((rand() % 15) + 85))/100;
            int criticalRandom = rand() % 255;
            idleMove = trainerCurrentPokemon->moveOne;
            int randMoveIdle = rand() % 2;
            if (randMoveIdle == 1 && trainerCurrentPokemon->numMoves > 1)
            {
                idleMove = trainerCurrentPokemon->moveTwo;
            }
            bool isCritical = criticalRandom < (trainerCurrentPokemon->baseSpeed/2);
            idleMoveDamage = ((((((2*trainerCurrentPokemon->level)/5) + 2) * trainerCurrentPokemon->moveOne.power * (trainerCurrentPokemon->attack/trainerCurrentPokemon->defense))/50) + 2) * attackRandom * typeEffective(idleMove.type_id, currentPokemon);
            if (isCritical)
            {
                idleMoveDamage *= 1.5;
            }
            if (idleMove.type_id == trainerCurrentPokemon->typeOne || idleMove.type_id == trainerCurrentPokemon->typeTwo)
            {
                idleMoveDamage *= 1.5;
            }
        }
        else
        {
            float attackRandom = ((float)((rand() % 15) + 85))/100;
            int criticalRandom = rand() % 255;
            idleMove = wildPokemon.moveOne;
            int randMoveIdle = rand() % 2;
            if (randMoveIdle == 1 && wildPokemon.numMoves > 1)
            {
                idleMove = wildPokemon.moveTwo;
            }
            bool isCritical = criticalRandom < (wildPokemon.baseSpeed/2);
            idleMoveDamage = ((((((2*wildPokemon.level)/5) + 2) * wildPokemon.moveOne.power * (wildPokemon.attack/wildPokemon.defense))/50) + 2) * attackRandom * typeEffective(idleMove.type_id, currentPokemon);
            if (isCritical)
            {
                idleMoveDamage *= 1.5;
            }
            if (idleMove.type_id == wildPokemon.typeOne || idleMove.type_id == wildPokemon.typeTwo)
            {
                idleMoveDamage *= 1.5;
            }
        }
        if (isIdleMove)
        {
            pokeWindowMessage2 = "Pokemon used " + idleMove.identifier + "(" + std::to_string(idleMoveDamage) + " Damage)";
            if (isTrainerBattle)
            {
                pokeWindowMessage2 = "Trainer used " + idleMove.identifier + "(" + std::to_string(idleMoveDamage) + " Damage)";
            }
            isIdleMove = false;
            currentPokemon->hp = currentPokemon->hp - idleMoveDamage;
            if (currentPokemon->hp < 0)
            {
                currentPokemon->hp = 0;
            }
            if (isAllKnockedOut(map, 0))
            {
                WINDOW *pcDefeatWindow = newwin(21, 80, 1, 0);
                mvwprintw(pcDefeatWindow, 10, 28, "You have been defeated");
                mvwprintw(pcDefeatWindow, 12, 28, "ESC to exit this screen");
                wrefresh(pcDefeatWindow);
                int escChar = getch();
                while (escChar != 27)
                {
                    escChar = getch();
                }
                escape = true;
                break;
            }
        }
        wclear(pokeWindow);
        mvwprintw(pokeWindow, 1, 25, "Press key (1), (2), (3), or (4)");
        mvwprintw(pokeWindow, 17, 59, "(PC's Pokemon)");
        if (isTrainerBattle)
        {
            printPokemonStats(pokeWindow, 3, 1, trainerCurrentPokemon);
            mvwprintw(pokeWindow, 0, 34, "Trainer Battle");
            mvwprintw(pokeWindow, 17, 0, "(Trainer's Pokemon)");
        }
        else
        {
            printPokemonStats(pokeWindow, 3, 1, &wildPokemon);
            mvwprintw(pokeWindow, 0, 33, "Pokemon Encounter");
            mvwprintw(pokeWindow, 17, 1, "(Wild Pokemon)");
        }
        printPokemonStats(pokeWindow, 3, 57, currentPokemon);
        mvwprintw(pokeWindow, 7, 32, "(1) Fight");
        mvwprintw(pokeWindow, 8, 32, "(2) Bag");
        mvwprintw(pokeWindow, 9, 32, "(3) Run");
        mvwprintw(pokeWindow, 10, 32, "(4) Switch Pokemon");
        mvwprintw(pokeWindow, 12, 22, stringToChar(pokeWindowMessage1));
        mvwprintw(pokeWindow, 13, 22, stringToChar(pokeWindowMessage2));
        wrefresh(pokeWindow);
        refresh();
        pokeWindowMessage1 = "";
        pokeWindowMessage2 = "";
        c = getch();
        if (c == '1' && currentPokemon->hp > 0) //fight
        {
            WINDOW *fightWindow = newwin(20, 29, 3, 26);
            int fightInput;
            bool exitFight = false;
            while (!exitFight)
            {
                mvwprintw(fightWindow, 0, 0, "Choose move (ESC to cancel)");
                std::string moveOneString = "(1) " + currentPokemon->moveOne.identifier;
                std::string moveTwoString = "(2) " + currentPokemon->moveTwo.identifier;
                float attackRandom = ((float)((rand() % 15) + 85))/100;
                int criticalRandom = rand() % 255;
                bool isCritical = criticalRandom < (currentPokemon->baseSpeed/2);
                int moveOneDamage = ((((((2*currentPokemon->level)/5) + 2) * currentPokemon->moveOne.power * (currentPokemon->attack/currentPokemon->defense))/50) + 2) * attackRandom * typeEffective(currentPokemon->moveOne.type_id, trainerCurrentPokemon);
                int moveTwoDamage = ((((((2*currentPokemon->level)/5) + 2) * currentPokemon->moveTwo.power * (currentPokemon->attack/currentPokemon->defense))/50) + 2) * attackRandom * typeEffective(currentPokemon->moveTwo.type_id, trainerCurrentPokemon);
                if (isCritical)
                {
                    moveOneDamage *= 1.5;
                    moveTwoDamage *= 1.5;
                }
                if (currentPokemon->moveOne.type_id == currentPokemon->typeOne || currentPokemon->moveOne.type_id == currentPokemon->typeTwo)
                {
                    moveOneDamage *= 1.5;
                }
                if (currentPokemon->moveTwo.type_id == currentPokemon->typeOne || currentPokemon->moveTwo.type_id == currentPokemon->typeTwo)
                {
                    moveTwoDamage *= 1.5;
                }
                std::string moveOneDamageString = "Damage: " + std::to_string(moveOneDamage);
                std::string moveTwoDamageString = "Damage: " + std::to_string(moveTwoDamage);
                mvwprintw(fightWindow, 2, 0, stringToChar(moveOneString));
                mvwprintw(fightWindow, 3, 0, stringToChar(moveOneDamageString));
                if (currentPokemon->numMoves > 1)
                {
                    mvwprintw(fightWindow, 5, 0, stringToChar(moveTwoString));
                    mvwprintw(fightWindow, 6, 0, stringToChar(moveTwoDamageString));
                }
                wrefresh(fightWindow);
                refresh();

                Moves selectedMove = currentPokemon->moveOne;
                int selectedMoveDamage = moveOneDamage;
                bool validInput = false;
                fightInput = getch();
                if (fightInput == 27)
                {
                    exitFight = true;
                }
                else if (fightInput == '1')
                {
                    selectedMove = currentPokemon->moveOne;
                    validInput = true;
                    exitFight = true;
                }
                else if (fightInput == '2' && currentPokemon->numMoves > 1)
                {
                    selectedMove = currentPokemon->moveTwo;
                    selectedMoveDamage = moveTwoDamage;
                    validInput = true;
                    exitFight = true;
                }

                if (validInput)
                {
                    int randHit = rand() % 100;
                    bool didHitPc = randHit < selectedMove.accuracy;
                    if (isTrainerBattle)
                    {
                        Moves trainerMove = trainerCurrentPokemon->moveOne;
                        int randMove = rand() % 2;
                        if (randMove == 1 && trainerCurrentPokemon->numMoves > 1)
                        {
                            trainerMove = trainerCurrentPokemon->moveTwo;
                        }
                        bool didHitTrainer = randHit < trainerMove.accuracy;
                        isCritical = criticalRandom < (trainerCurrentPokemon->baseSpeed/2);
                        int trainerMoveDamage = ((((((2*trainerCurrentPokemon->level)/5) + 2) * trainerCurrentPokemon->moveOne.power * (trainerCurrentPokemon->attack/trainerCurrentPokemon->defense))/50) + 2) * attackRandom * typeEffective(trainerMove.type_id, currentPokemon);
                        if (isCritical)
                        {
                            trainerMoveDamage *= 1.5;
                        }
                        if (trainerMove.type_id == trainerCurrentPokemon->typeOne || trainerMove.type_id == trainerCurrentPokemon->typeTwo)
                        {
                            trainerMoveDamage *= 1.5;
                        }

                        if (didHitPc && didHitTrainer)
                        {
                            pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                            pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                            if (selectedMove.priority > trainerMove.priority)
                            {
                                //pc move goes first
                                trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                                if (trainerCurrentPokemon->hp <= 0)
                                {
                                    trainerCurrentPokemon->hp = 0;
                                    if (!isAllKnockedOut(map, trainerIdx))
                                    {
                                        for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                        {
                                            if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                            {
                                                trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                            else if (selectedMove.priority < trainerMove.priority)
                            {
                                // trainer move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                                currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                                if (currentPokemon->hp < 0)
                                {
                                    currentPokemon->hp = 0;
                                }
                                else
                                {
                                    trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                                    if (trainerCurrentPokemon->hp <= 0)
                                    {
                                        trainerCurrentPokemon->hp = 0;
                                        if (!isAllKnockedOut(map, trainerIdx))
                                        {
                                            for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                            {
                                                if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                                {
                                                    trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else if (currentPokemon->speed > trainerCurrentPokemon->speed)
                            {
                                //pc move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                                trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                                if (trainerCurrentPokemon->hp <= 0)
                                {
                                    trainerCurrentPokemon->hp = 0;
                                    if (!isAllKnockedOut(map, trainerIdx))
                                    {
                                        for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                        {
                                            if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                            {
                                                trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                            else if (currentPokemon->speed < trainerCurrentPokemon->speed)
                            {
                                // trainer move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                                currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                                if (currentPokemon->hp < 0)
                                {
                                    currentPokemon->hp = 0;
                                }
                                else
                                {
                                    trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                                    if (trainerCurrentPokemon->hp <= 0)
                                    {
                                        trainerCurrentPokemon->hp = 0;
                                        if (!isAllKnockedOut(map, trainerIdx))
                                        {
                                            for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                            {
                                                if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                                {
                                                    trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                            else
                            {
                                // pc move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                                trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                                if (trainerCurrentPokemon->hp <= 0)
                                {
                                    trainerCurrentPokemon->hp = 0;
                                    if (!isAllKnockedOut(map, trainerIdx))
                                    {
                                        for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                        {
                                            if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                            {
                                                trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                            }
                                        }
                                    }
                                }
                                else
                                {
                                    currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                        }
                        else if (!didHitPc && didHitTrainer)
                        {
                            pokeWindowMessage1 = "PC's move missed.";
                            pokeWindowMessage2 = "Trainer used " + trainerMove.identifier + "(" + std::to_string(trainerMoveDamage) + " Damage)";
                            currentPokemon->hp = currentPokemon->hp - trainerMoveDamage;
                            if (currentPokemon->hp < 0)
                            {
                                currentPokemon->hp = 0;
                            }
                        }
                        else if (didHitPc && !didHitTrainer)
                        {
                            pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                            pokeWindowMessage2 = "Trainer's move missed.";
                            trainerCurrentPokemon->hp = trainerCurrentPokemon->hp - selectedMoveDamage;
                            if (trainerCurrentPokemon->hp <= 0)
                            {
                                trainerCurrentPokemon->hp = 0;
                                if (!isAllKnockedOut(map, trainerIdx))
                                {
                                    for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                    {
                                        if (map->trainers[trainerIdx].pokemonList[i].hp > 0)
                                        {
                                            trainerCurrentPokemon = &map->trainers[trainerIdx].pokemonList[i];
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {
                            pokeWindowMessage1 = "PC's move missed.";
                            pokeWindowMessage2 = "Trainer's move missed.";
                        }

                        if (isAllKnockedOut(map, trainerIdx) || isAllKnockedOut(map, 0))
                        {
                            WINDOW *trainerDefeatWindow = newwin(21, 80, 1, 0);
                            if (isAllKnockedOut(map, trainerIdx))
                            {
                                mvwprintw(trainerDefeatWindow, 10, 30, "Trainer defeated");
                                int pokeBucksGained = 0;
                                for (int i = 0; i < map->trainers[trainerIdx].numPokemon; i++)
                                {
                                    pokeBucksGained += rand() % 100;
                                }
                                map->trainers[0].pokeBucks += pokeBucksGained;
                                std::string pokeBucksString = "PokeBucks Gained: " + std::to_string(pokeBucksGained);
                                mvwprintw(trainerDefeatWindow, 12, 28, stringToChar(pokeBucksString));
                            }
                            else
                            {
                                mvwprintw(trainerDefeatWindow, 10, 28, "You have been defeated");
                            }
                            mvwprintw(trainerDefeatWindow, 14, 28, "ESC to exit this screen");
                            wrefresh(trainerDefeatWindow);
                            int escChar = getch();
                            while (escChar != 27)
                            {
                                escChar = getch();
                            }
                            escape = true;
                            break;
                        }
                    }
                    else
                    {
                        // wild pokemon battle
                        Moves wildPokemonMove = wildPokemon.moveOne;
                        int randMove = rand() % 2;
                        if (randMove == 1 && wildPokemon.numMoves > 1)
                        {
                            wildPokemonMove = wildPokemon.moveTwo;
                        }
                        bool didHitWild = randHit < wildPokemonMove.accuracy;
                        bool isPokemonDefeated = false;
                        isCritical = criticalRandom < (wildPokemon.baseSpeed/2);
                        int wildPokemonMoveDamage = ((((((2*wildPokemon.level)/5) + 2) * wildPokemon.moveOne.power * (wildPokemon.attack/wildPokemon.defense))/50) + 2) * attackRandom * typeEffective(wildPokemonMove.type_id, currentPokemon);
                        if (isCritical)
                        {
                            wildPokemonMoveDamage *= 1.5;
                        }
                        if (wildPokemonMove.type_id == currentPokemon->typeOne || wildPokemonMove.type_id == currentPokemon->typeTwo)
                        {
                            wildPokemonMoveDamage *= 1.5;
                        }
                        if (didHitPc && didHitWild)
                        {
                            pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                            pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                            if (selectedMove.priority > wildPokemonMove.priority)
                            {
                                //pc move goes first
                                wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                                isPokemonDefeated = wildPokemon.hp <= 0;
                                if (!isPokemonDefeated)
                                {
                                    currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                            else if (selectedMove.priority < wildPokemonMove.priority)
                            {
                                // pokemon move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                                currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                                if (currentPokemon->hp < 0)
                                {
                                    currentPokemon->hp = 0;
                                }
                                else
                                {
                                    wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                                    isPokemonDefeated = wildPokemon.hp <= 0;
                                }
                            }
                            else if (currentPokemon->speed > wildPokemon.speed)
                            {
                                //pc move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                                wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                                isPokemonDefeated = wildPokemon.hp <= 0;
                                if (!isPokemonDefeated)
                                {
                                    currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                            else if (currentPokemon->speed < wildPokemon.speed)
                            {
                                // pokemon move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                                currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                                if (currentPokemon->hp < 0)
                                {
                                    currentPokemon->hp = 0;
                                }
                                else
                                {
                                    wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                                    isPokemonDefeated = wildPokemon.hp <= 0;
                                }
                            }
                            else
                            {
                                // pc move goes first
                                pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                                pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                                wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                                isPokemonDefeated = wildPokemon.hp <= 0;
                                if (!isPokemonDefeated)
                                {
                                    currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                                    if (currentPokemon->hp < 0)
                                    {
                                        currentPokemon->hp = 0;
                                    }
                                }
                            }
                        }
                        else if (!didHitPc && didHitWild)
                        {
                            pokeWindowMessage1 = "PC's move missed.";
                            pokeWindowMessage2 = "Pokemon used " + wildPokemonMove.identifier + "(" + std::to_string(wildPokemonMoveDamage) + " Damage)";
                            currentPokemon->hp = currentPokemon->hp - wildPokemonMoveDamage;
                            if (currentPokemon->hp < 0)
                            {
                                currentPokemon->hp = 0;
                            }
                        }
                        else if (didHitPc && !didHitWild)
                        {
                            pokeWindowMessage1 = "PC used " + selectedMove.identifier + "(" + std::to_string(selectedMoveDamage) + " Damage)";
                            pokeWindowMessage2 = "Pokemon's move missed.";
                            wildPokemon.hp = wildPokemon.hp - selectedMoveDamage;
                            isPokemonDefeated = wildPokemon.hp <= 0;
                        }
                        else
                        {
                            pokeWindowMessage1 = "PC's move missed.";
                            pokeWindowMessage2 = "Pokemon's move missed.";
                        }

                        if (isPokemonDefeated || isAllKnockedOut(map, 0))
                        {
                            WINDOW *pokemonDefeatWindow = newwin(21, 80, 1, 0);
                            if (isPokemonDefeated)
                            {
                                mvwprintw(pokemonDefeatWindow, 10, 30, "Pokemon defeated");
                            }
                            else
                            {
                                mvwprintw(pokemonDefeatWindow, 10, 28, "You have been defeated");
                            }
                            mvwprintw(pokemonDefeatWindow, 12, 28, "ESC to exit this screen");
                            wrefresh(pokemonDefeatWindow);
                            int escChar = getch();
                            while (escChar != 27)
                            {
                                escChar = getch();
                            }
                            escape = true;
                            break;
                        }
                    }
                }
            }
            wclear(fightWindow);
            wrefresh(fightWindow);
        }
        else if (c == '1' && currentPokemon->hp == 0)
        {
            mvwprintw(pokeWindow, 12, 26, "Cannot fight at 0 HP!");
        }
        else if (c == '2') //bag
        {
            WINDOW *bagWindow = newwin(20, 29, 3, 26);
            int bagInput;
            bool exitBag = false;
            while (!exitBag)
            {
                mvwprintw(bagWindow, 0, 0, "PC's Bag (ESC to Exit)");
                std::string pokeballsString = "(1) Pokeballs: " + std::to_string(map->trainers[0].numPokeballs);
                std::string revivesString = "(2) Revives: " + std::to_string(map->trainers[0].numRevives);
                std::string potionsString = "(3) Potions: " + std::to_string(map->trainers[0].numPotions);
                mvwprintw(bagWindow, 2, 0, stringToChar(pokeballsString));
                mvwprintw(bagWindow, 3, 0, stringToChar(revivesString));
                mvwprintw(bagWindow, 4, 0, stringToChar(potionsString));
                wrefresh(bagWindow);
                refresh();
                bagInput = getch();
                if (bagInput == 27)
                {
                    exitBag = true;
                }
                else if (bagInput == '1' && !isTrainerBattle) //pokeball
                {
                    int m = rand() % 255;
                    int f = (wildPokemon.maxhp * 255 * 4)/(wildPokemon.hp * 8);
                    map->trainers[0].numPokeballs = map->trainers[0].numPokeballs - 1;
                    if (f >= m && map->trainers[0].numPokeballs > 0)
                    {
                        escape = true;
                        exitBag = true;
                        WINDOW *captureWindow = newwin(21, 80, 1, 0);
                        std::string captureString = "Successfully captured " + wildPokemon.name;
                        mvwprintw(captureWindow, 10, 25, stringToChar(captureString));
                        mvwprintw(captureWindow, 12, 28, "ESC to exit this screen");
                        wildPokemon.hp = wildPokemon.maxhp;
                        if (map->trainers[0].numPokemon < 6)
                        {
                            map->trainers[0].pokemonList[map->trainers[0].numPokemon] = wildPokemon;
                            map->trainers[0].numPokemon = map->trainers[0].numPokemon + 1;
                        }
                        else
                        {
                            mvwprintw(captureWindow, 11, 26, "Pokemon sent to storage.");
                            map->pokeStorage[map->numPokemonStorage] = wildPokemon;
                            map->numPokemonStorage++;
                        }
                        wrefresh(captureWindow);
                        int escChar = getch();
                        while (escChar != 27)
                        {
                            escChar = getch();
                        }
                    }
                    else
                    {
                        WINDOW *pokemonEscapedWindow = newwin(21, 80, 1, 0);
                        mvwprintw(pokemonEscapedWindow, 10, 28, "The Pokemon has escaped!");
                        mvwprintw(pokemonEscapedWindow, 12, 28, "ESC to exit this screen");
                        wrefresh(pokemonEscapedWindow);
                        int escChar = getch();
                        while (escChar != 27)
                        {
                            escChar = getch();
                        }
                        escape = true;
                        break;
                    }
                }
                else if (bagInput == '2') //revive
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                        mvwprintw(bagWindow, 18, 0, "Cannot use Revive!");
                        wrefresh(bagWindow);
                    }
                }
                else if (bagInput == '3') //potion
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                                    isIdleMove = true;
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
                        mvwprintw(bagWindow, 18, 0, "Cannot use Potion!");
                        wrefresh(bagWindow);
                    }
                }
            }
            wclear(bagWindow);
            wrefresh(bagWindow);
        }
        else if (c == '3' && !isTrainerBattle) //run
        {
            isIdleMove = true;
            numRunAttempts++;
            int randEscape = rand() % 256;
            if (randEscape < (((currentPokemon->speed * 32)/((wildPokemon.speed/4) % 256)) + 30 * numRunAttempts))
            {
                WINDOW *successRunWindow = newwin(21, 80, 1, 0);
                mvwprintw(successRunWindow, 10, 32, "You have escaped!");
                mvwprintw(successRunWindow, 12, 28, "ESC to exit this screen");
                wrefresh(successRunWindow);
                int escChar = getch();
                while (escChar != 27)
                {
                    escChar = getch();
                }
                escape = true;
                break;
            }
        }
        else if (c == '4') //switch pokemon
        {
            WINDOW *switchPokemon = newwin(20, 29, 3, 26);
            mvwprintw(switchPokemon, 0, 0, "Switch Pokemon (ESC to Exit)");
            for (int i = 0; i < map->trainers[0].numPokemon; i++)
            {
                std::string s = "(" + std::to_string(i+1) + ") " + map->trainers[0].pokemonList[i].name;
                mvwprintw(switchPokemon, (i + 2), 0, stringToChar(s));
            }
            wrefresh(switchPokemon);
            refresh();
            int switchPokeInput;
            int inputToInt = -1;
            bool selected = false;
            while (!selected)
            {
                switchPokeInput = getch();
                if (switchPokeInput == 27)
                {
                    selected = true;
                }
                else if (switchPokeInput == '1')
                {
                    inputToInt = 0;
                }
                else if (map->trainers[0].numPokemon >= 2 && switchPokeInput == '2')
                {
                    inputToInt = 1;
                }
                else if (map->trainers[0].numPokemon >= 3 && switchPokeInput == '3')
                {
                    inputToInt = 2;
                }
                else if (map->trainers[0].numPokemon >= 4 && switchPokeInput == '4')
                {
                    inputToInt = 3;
                }
                else if (map->trainers[0].numPokemon >= 5 && switchPokeInput == '5')
                {
                    inputToInt = 4;
                }
                else if (map->trainers[0].numPokemon == 6 && switchPokeInput == '6')
                {
                    inputToInt = 5;
                }

                if (inputToInt != -1 && map->trainers[0].pokemonList[inputToInt].hp > 0
                    && map->trainers[0].pokemonList[inputToInt].name != currentPokemon->name)
                {
                    currentPokemon = &map->trainers[0].pokemonList[inputToInt];
                    selected = true;
                    isIdleMove = true;
                }
                else if (inputToInt != -1)
                {
                    mvwprintw(switchPokemon, 10, 0, "Cannot switch to this Pokemon");
                    wrefresh(switchPokemon);
                }
            }
            wclear(switchPokemon);
            wrefresh(switchPokemon);
        }
    }
}

int main(int argc, char *argv[])
{
    std::string filename;
    std::string beginPath;
    std::string line;
    if (std::filesystem::is_directory("/home/angelo/coms327/pokefinal/pokedex"))
    {
        beginPath = "/home/angelo/coms327/pokefinal/pokedex/";
    }
    std::ifstream input;
    AllPokemonArrays *allArrays = new AllPokemonArrays();
    filename = beginPath + "pokemon.csv";
    input.open(filename);
    int lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->pokemonList = new Pokemon[lineCount]();
    allArrays->pokemonListLength = lineCount;
    parsePokemonList(allArrays, filename);

    filename = beginPath + "moves.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->movesList = new Moves[lineCount]();
    allArrays->movesListLength = lineCount;
    parseMovesList(allArrays, filename);

    filename = beginPath + "pokemon_moves.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->pokemonMovesList = new Pokemon_moves[lineCount]();
    allArrays->pokemonMovesListLength = lineCount;
    parsePokemonMovesList(allArrays, filename);

    filename = beginPath + "pokemon_species.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->pokemonSpeciesList = new Pokemon_species[lineCount]();
    allArrays->pokemonSpeciesListLength = lineCount;
    parsePokemonSpeciesList(allArrays, filename);

    filename = beginPath + "pokemon_types.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->pokemonTypesList = new Pokemon_types[lineCount]();
    allArrays->pokemonTypesListLength = lineCount;
    parsePokemonTypesList(allArrays, filename);

    filename = beginPath + "experience.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->experienceList = new Experience[lineCount]();
    allArrays->experienceListLength = lineCount;
    parseExperienceList(allArrays, filename);

    filename = beginPath + "pokemon_stats.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->pokemonStatsList = new Pokemon_stats[lineCount]();
    allArrays->pokemonStatsListLength = lineCount;
    parsePokemonStatsList(allArrays, filename);

    filename = beginPath + "type_names.csv";
    input.open(filename);
    lineCount = -2;
    while (input)
    {
        std::getline(input, line);
        lineCount++;
    }
    input.close();

    allArrays->typeNamesList = new Type_names[lineCount]();
    allArrays->typeNamesListLength = lineCount;
    parseTypeNamesList(allArrays, filename);

    srand(time(NULL));
    struct Map *allMaps[399][399];
    int numTrainers = 10;
    if (argc == 3)
    {
        std::string argOne = argv[1];
        std::string argTwo = argv[2];
        if (argOne == "--numtrainers" && checkIfNumber(argTwo))
        {
            numTrainers = std::stoi(argTwo);
        }
    }
    for (int i = 0; i < 399; i++)
    {
        for (int j = 0; j < 399; j++)
        {
            allMaps[i][j] = new Map();
        }
    }
    int currentXmap = 199;
    int currentYmap = 199;
    char map[21][80];
    generateMap(map, allMaps, currentXmap, currentYmap);
    copy2dArrayToPointer(map, allMaps, currentXmap, currentYmap);
    int randXpc = (rand() % 78) + 1;
    for (int y = 1; y < 20; y++)
    {
        if (allMaps[currentXmap][currentYmap]->built_map[y][randXpc] == '#')
        {
            allMaps[currentXmap][currentYmap]->pcPos[dim_x] = randXpc;
            allMaps[currentXmap][currentYmap]->pcPos[dim_y] = y;
            allMaps[currentXmap][currentYmap]->built_map[y][randXpc] = '@';
            break;
        }
    }
    initializeTrainers(allMaps, currentXmap, currentYmap, numTrainers, allArrays);
    allMaps[currentXmap][currentYmap]->trainers[0].numPokeballs = 10;
    allMaps[currentXmap][currentYmap]->trainers[0].numPotions = 10;
    allMaps[currentXmap][currentYmap]->trainers[0].numRevives = 10;
    allMaps[currentXmap][currentYmap]->trainers[0].pokeBucks = 0;
    allMaps[currentXmap][currentYmap]->numPokemonStorage = 0;
    allMaps[currentXmap][currentYmap]->pokeStorage = new pokemon_class[500]();
    initscr();
    noecho();
    cbreak();
    start_color();
    keypad(stdscr,TRUE);
    init_pair(1, COLOR_GREEN, COLOR_BLACK); // Tall Grass
    init_pair(2, COLOR_RED, COLOR_BLACK); // Boulders/Trees
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK); // PokeMart/PokeCenter
    init_pair(4, COLOR_CYAN, COLOR_BLACK); // Trainers
    init_pair(5, COLOR_BLUE, COLOR_BLACK); // Path
    init_pair(6, COLOR_YELLOW, COLOR_BLACK); // Clearings
    init_pair(7, COLOR_WHITE, COLOR_BLACK); // PC
    printw("Messages: \n");

    mvprintw(1, 16, "Choose your starting Pokemon by pressing (1), (2), or (3)");
    pokemon_class pokemon1;
    pokemon_class pokemon2;
    pokemon_class pokemon3;
    generatePokemon(&pokemon1, allArrays, allMaps[currentXmap][currentYmap]);
    generatePokemon(&pokemon2, allArrays, allMaps[currentXmap][currentYmap]);
    generatePokemon(&pokemon3, allArrays, allMaps[currentXmap][currentYmap]);
    while (pokemon1.name == pokemon2.name || pokemon1.name == pokemon3.name || pokemon2.name == pokemon3.name)
    {
        generatePokemon(&pokemon1, allArrays, allMaps[currentXmap][currentYmap]);
        generatePokemon(&pokemon2, allArrays, allMaps[currentXmap][currentYmap]);
        generatePokemon(&pokemon3, allArrays, allMaps[currentXmap][currentYmap]);
    }
    std::string pokemonNameString = "Name: " + pokemon1.name;
    mvprintw(3, 1, stringToChar(pokemonNameString));
    std::string levelString = "Level: " + std::to_string(pokemon1.level);
    mvprintw(4, 1, stringToChar(levelString));
    std::string genderString = "Gender: " + pokemon1.gender;
    mvprintw(5, 1, stringToChar(genderString));
    std::string isShinyString = "Shiny: ";
    if (pokemon1.isShiny)
    {
        isShinyString = isShinyString + "Yes";
    }
    else
    {
        isShinyString = isShinyString + "No";
    }
    mvprintw(6, 1, stringToChar(isShinyString));
    std::string moveOneString = "Move 1: " + pokemon1.moveOne.identifier;
    std::string moveTwoString = "Move 2: " + pokemon1.moveTwo.identifier;
    mvprintw(8, 1, stringToChar(moveOneString));
    mvprintw(9, 1, stringToChar(moveTwoString));
    std::string hpString = "HP: " + std::to_string(pokemon1.hp);
    mvprintw(10, 1, stringToChar(hpString));
    std::string attackString = "Attack: " + std::to_string(pokemon1.attack);
    mvprintw(11, 1, stringToChar(attackString));
    std::string defenseString = "Defense: " + std::to_string(pokemon1.defense);
    mvprintw(12, 1, stringToChar(defenseString));
    std::string specAttackString = "Special Attack: " + std::to_string(pokemon1.specAttack);
    mvprintw(13, 1, stringToChar(specAttackString));
    std::string specDefenseString = "Special Defense: " + std::to_string(pokemon1.specDefense);
    mvprintw(14, 1, stringToChar(specDefenseString));
    std::string speedString = "Speed: " + std::to_string(pokemon1.speed);
    mvprintw(15, 1, stringToChar(speedString));

    pokemonNameString = "Name: " + pokemon2.name;
    mvprintw(3, 32, stringToChar(pokemonNameString));
    levelString = "Level: " + std::to_string(pokemon2.level);
    mvprintw(4, 32, stringToChar(levelString));
    genderString = "Gender: " + pokemon2.gender;
    mvprintw(5, 32, stringToChar(genderString));
    isShinyString = "Shiny: ";
    if (pokemon2.isShiny)
    {
        isShinyString = isShinyString + "Yes";
    }
    else
    {
        isShinyString = isShinyString + "No";
    }
    mvprintw(6, 32, stringToChar(isShinyString));
    moveOneString = "Move 1: " + pokemon2.moveOne.identifier;
    moveTwoString = "Move 2: " + pokemon2.moveTwo.identifier;
    mvprintw(8, 32, stringToChar(moveOneString));
    mvprintw(9, 32, stringToChar(moveTwoString));
    hpString = "HP: " + std::to_string(pokemon2.hp);
    mvprintw(10, 32, stringToChar(hpString));
    attackString = "Attack: " + std::to_string(pokemon2.attack);
    mvprintw(11, 32, stringToChar(attackString));
    defenseString = "Defense: " + std::to_string(pokemon2.defense);
    mvprintw(12, 32, stringToChar(defenseString));
    specAttackString = "Special Attack: " + std::to_string(pokemon2.specAttack);
    mvprintw(13, 32, stringToChar(specAttackString));
    specDefenseString = "Special Defense: " + std::to_string(pokemon2.specDefense);
    mvprintw(14, 32, stringToChar(specDefenseString));
    speedString = "Speed: " + std::to_string(pokemon2.speed);
    mvprintw(15, 32, stringToChar(speedString));

    pokemonNameString = "Name: " + pokemon3.name;
    mvprintw(3, 60, stringToChar(pokemonNameString));
    levelString = "Level: " + std::to_string(pokemon3.level);
    mvprintw(4, 60, stringToChar(levelString));
    genderString = "Gender: " + pokemon3.gender;
    mvprintw(5, 60, stringToChar(genderString));
    isShinyString = "Shiny: ";
    if (pokemon3.isShiny)
    {
        isShinyString = isShinyString + "Yes";
    }
    else
    {
        isShinyString = isShinyString + "No";
    }
    mvprintw(6, 60, stringToChar(isShinyString));
    moveOneString = "Move 1: " + pokemon3.moveOne.identifier;
    moveTwoString = "Move 2: " + pokemon3.moveTwo.identifier;
    mvprintw(8, 60, stringToChar(moveOneString));
    mvprintw(9, 60, stringToChar(moveTwoString));
    hpString = "HP: " + std::to_string(pokemon3.hp);
    mvprintw(10, 60, stringToChar(hpString));
    attackString = "Attack: " + std::to_string(pokemon3.attack);
    mvprintw(11, 60, stringToChar(attackString));
    defenseString = "Defense: " + std::to_string(pokemon3.defense);
    mvprintw(12, 60, stringToChar(defenseString));
    specAttackString = "Special Attack: " + std::to_string(pokemon3.specAttack);
    mvprintw(13, 60, stringToChar(specAttackString));
    specDefenseString = "Special Defense: " + std::to_string(pokemon3.specDefense);
    mvprintw(14, 60, stringToChar(specDefenseString));
    speedString = "Speed: " + std::to_string(pokemon3.speed);
    mvprintw(15, 60, stringToChar(speedString));
    mvprintw(17, 5, "(1)");
    mvprintw(17, 36, "(2)");
    mvprintw(17, 64, "(3)");
    refresh();
    int cInput;
    bool escape = false;
    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList = new pokemon_class[6]();
    while (!escape)
    {
        cInput = getch();
        if (cInput == '1')
        {
            allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[0] = pokemon1;
            escape = true;
        }
        else if (cInput == '2')
        {
            allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[0] = pokemon2;
            escape = true;
        }
        else if (cInput == '3')
        {
            allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[0] = pokemon3;
            escape = true;
        }
    }

    allMaps[currentXmap][currentYmap]->trainers[0].numPokemon = 1;
    drawScreen(allMaps[currentXmap][currentYmap], currentXmap, currentYmap);
    refresh();
    moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 1, "--", currentXmap, currentYmap);
    int quit_game = 0;
    int validKey = 1;
    while (quit_game == 0)
    {
        cInput = getch();
        validKey = 1;
        std::string pcDir = "--";
        pcDir = getMovementInput(cInput, pcDir, allMaps[currentXmap][currentYmap], allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, currentXmap, currentYmap);
        // qq = quit, vv = invalid input
        if (pcDir == "vv")
        {
            validKey = 0;
        }
        else if (pcDir == "qq")
        {
            quit_game = 1;
            endwin();
            return 0;
        }
        if (cInput != 'Q' && validKey == 1)
        {
            moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 0, pcDir, currentXmap, currentYmap);
            for (int i = 0; i < 69; i++)
            {
                mvdelch(0, 10);
            }

            if (allMaps[currentXmap][currentYmap]->pcPos[dim_x] == 0)
            {
                currentXmap--;
                allMaps[currentXmap][currentYmap]->pcPos[dim_x] = 78;
                allMaps[currentXmap][currentYmap]->pcPos[dim_y] = allMaps[currentXmap+1][currentYmap]->pcPos[dim_y];
                if (allMaps[currentXmap][currentYmap]->built_map[0][0] != '%')
                {
                    char map[21][80];
                    generateMap(map, allMaps, currentXmap, currentYmap);
                    copy2dArrayToPointer(map, allMaps, currentXmap, currentYmap);
                    initializeTrainers(allMaps, currentXmap, currentYmap, numTrainers, allArrays);
                    moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 1, "--", currentXmap, currentYmap);
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList = new pokemon_class[6]();
                }
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][79] = '#';
                allMaps[currentXmap][currentYmap]->trainers[0].trainerPos[dim_x] = 78;
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '@';
                allMaps[currentXmap][currentYmap]->trainers[0].numPokemon = allMaps[currentXmap+1][currentYmap]->trainers[0].numPokemon;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->trainers[0].numPokemon; i++)
                {
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[i] = allMaps[currentXmap+1][currentYmap]->trainers[0].pokemonList[i];
                }
                allMaps[currentXmap][currentYmap]->trainers[0].numPokeballs = allMaps[currentXmap+1][currentYmap]->trainers[0].numPokeballs;
                allMaps[currentXmap][currentYmap]->trainers[0].numPotions = allMaps[currentXmap+1][currentYmap]->trainers[0].numPotions;
                allMaps[currentXmap][currentYmap]->trainers[0].numRevives = allMaps[currentXmap+1][currentYmap]->trainers[0].numRevives;
                allMaps[currentXmap][currentYmap]->trainers[0].pokeBucks = allMaps[currentXmap+1][currentYmap]->trainers[0].pokeBucks;
                allMaps[currentXmap][currentYmap]->numPokemonStorage = allMaps[currentXmap+1][currentYmap]->numPokemonStorage;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->numPokemonStorage; i++)
                {
                    allMaps[currentXmap][currentYmap]->pokeStorage[i] = allMaps[currentXmap+1][currentYmap]->pokeStorage[i];
                }
            }
            else if (allMaps[currentXmap][currentYmap]->pcPos[dim_x] == 79)
            {
                currentXmap++;
                allMaps[currentXmap][currentYmap]->pcPos[dim_x] = 1;
                allMaps[currentXmap][currentYmap]->pcPos[dim_y] = allMaps[currentXmap-1][currentYmap]->pcPos[dim_y];
                if (allMaps[currentXmap][currentYmap]->built_map[0][0] != '%')
                {
                    char map[21][80];
                    generateMap(map, allMaps, currentXmap, currentYmap);
                    copy2dArrayToPointer(map, allMaps, currentXmap, currentYmap);
                    initializeTrainers(allMaps, currentXmap, currentYmap, numTrainers, allArrays);
                    moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 1, "--", currentXmap, currentYmap);
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList = new pokemon_class[6]();
                }
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][0] = '#';
                allMaps[currentXmap][currentYmap]->trainers[0].trainerPos[dim_x] = 1;
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '@';
                allMaps[currentXmap][currentYmap]->trainers[0].numPokemon = allMaps[currentXmap-1][currentYmap]->trainers[0].numPokemon;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->trainers[0].numPokemon; i++)
                {
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[i] = allMaps[currentXmap-1][currentYmap]->trainers[0].pokemonList[i];
                }
                allMaps[currentXmap][currentYmap]->trainers[0].numPokeballs = allMaps[currentXmap-1][currentYmap]->trainers[0].numPokeballs;
                allMaps[currentXmap][currentYmap]->trainers[0].numPotions = allMaps[currentXmap-1][currentYmap]->trainers[0].numPotions;
                allMaps[currentXmap][currentYmap]->trainers[0].numRevives = allMaps[currentXmap-1][currentYmap]->trainers[0].numRevives;
                allMaps[currentXmap][currentYmap]->trainers[0].pokeBucks = allMaps[currentXmap-1][currentYmap]->trainers[0].pokeBucks;
                allMaps[currentXmap][currentYmap]->numPokemonStorage = allMaps[currentXmap-1][currentYmap]->numPokemonStorage;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->numPokemonStorage; i++)
                {
                    allMaps[currentXmap][currentYmap]->pokeStorage[i] = allMaps[currentXmap-1][currentYmap]->pokeStorage[i];
                }
            }
            else if (allMaps[currentXmap][currentYmap]->pcPos[dim_y] == 0)
            {
                currentYmap++;
                allMaps[currentXmap][currentYmap]->pcPos[dim_x] = allMaps[currentXmap][currentYmap-1]->pcPos[dim_x];
                allMaps[currentXmap][currentYmap]->pcPos[dim_y] = 19;
                if (allMaps[currentXmap][currentYmap]->built_map[0][0] != '%')
                {
                    char map[21][80];
                    generateMap(map, allMaps, currentXmap, currentYmap);
                    copy2dArrayToPointer(map, allMaps, currentXmap, currentYmap);
                    initializeTrainers(allMaps, currentXmap, currentYmap, numTrainers, allArrays);
                    moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 1, "--", currentXmap, currentYmap);
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList = new pokemon_class[6]();
                }
                allMaps[currentXmap][currentYmap]->built_map[20][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '#';
                allMaps[currentXmap][currentYmap]->trainers[0].trainerPos[dim_y] = 19;
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '@';
                allMaps[currentXmap][currentYmap]->trainers[0].numPokemon = allMaps[currentXmap][currentYmap-1]->trainers[0].numPokemon;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->trainers[0].numPokemon; i++)
                {
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[i] = allMaps[currentXmap][currentYmap-1]->trainers[0].pokemonList[i];
                }
                allMaps[currentXmap][currentYmap]->trainers[0].numPokeballs = allMaps[currentXmap][currentYmap-1]->trainers[0].numPokeballs;
                allMaps[currentXmap][currentYmap]->trainers[0].numPotions = allMaps[currentXmap][currentYmap-1]->trainers[0].numPotions;
                allMaps[currentXmap][currentYmap]->trainers[0].numRevives = allMaps[currentXmap][currentYmap-1]->trainers[0].numRevives;
                allMaps[currentXmap][currentYmap]->trainers[0].pokeBucks = allMaps[currentXmap][currentYmap-1]->trainers[0].pokeBucks;
                allMaps[currentXmap][currentYmap]->numPokemonStorage = allMaps[currentXmap][currentYmap-1]->numPokemonStorage;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->numPokemonStorage; i++)
                {
                    allMaps[currentXmap][currentYmap]->pokeStorage[i] = allMaps[currentXmap][currentYmap-1]->pokeStorage[i];
                }
            }
            else if (allMaps[currentXmap][currentYmap]->pcPos[dim_y] == 20)
            {
                currentYmap--;
                allMaps[currentXmap][currentYmap]->pcPos[dim_x] = allMaps[currentXmap][currentYmap+1]->pcPos[dim_x];
                allMaps[currentXmap][currentYmap]->pcPos[dim_y] = 1;
                if (allMaps[currentXmap][currentYmap]->built_map[0][0] != '%')
                {
                    char map[21][80];
                    generateMap(map, allMaps, currentXmap, currentYmap);
                    copy2dArrayToPointer(map, allMaps, currentXmap, currentYmap);
                    initializeTrainers(allMaps, currentXmap, currentYmap, numTrainers, allArrays);
                    moveTrainers(allMaps[currentXmap][currentYmap]->trainers, numTrainers+1, allMaps[currentXmap][currentYmap], 1, "--", currentXmap, currentYmap);
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList = new pokemon_class[6]();
                }
                allMaps[currentXmap][currentYmap]->built_map[0][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '#';
                allMaps[currentXmap][currentYmap]->trainers[0].trainerPos[dim_y] = 1;
                allMaps[currentXmap][currentYmap]->built_map[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] = '@';
                allMaps[currentXmap][currentYmap]->trainers[0].numPokemon = allMaps[currentXmap][currentYmap+1]->trainers[0].numPokemon;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->trainers[0].numPokemon; i++)
                {
                    allMaps[currentXmap][currentYmap]->trainers[0].pokemonList[i] = allMaps[currentXmap][currentYmap+1]->trainers[0].pokemonList[i];
                }
                allMaps[currentXmap][currentYmap]->trainers[0].numPokeballs = allMaps[currentXmap][currentYmap+1]->trainers[0].numPokeballs;
                allMaps[currentXmap][currentYmap]->trainers[0].numPotions = allMaps[currentXmap][currentYmap+1]->trainers[0].numPotions;
                allMaps[currentXmap][currentYmap]->trainers[0].numRevives = allMaps[currentXmap][currentYmap+1]->trainers[0].numRevives;
                allMaps[currentXmap][currentYmap]->trainers[0].pokeBucks = allMaps[currentXmap][currentYmap+1]->trainers[0].pokeBucks;
                allMaps[currentXmap][currentYmap]->numPokemonStorage = allMaps[currentXmap][currentYmap+1]->numPokemonStorage;
                for (int i = 0; i < allMaps[currentXmap][currentYmap]->numPokemonStorage; i++)
                {
                    allMaps[currentXmap][currentYmap]->pokeStorage[i] = allMaps[currentXmap][currentYmap+1]->pokeStorage[i];
                }
            }
            for (int i = 1; i < numTrainers+1; i++)
            {
                static trainer_t *t;
                t = allMaps[currentXmap][currentYmap]->trainers;
                struct Map *map = allMaps[currentXmap][currentYmap];
                if (t[i].reachedPC == 0 && ((t[i].trainerNextPos[dim_x] == map->pcPos[dim_x] && t[i].trainerNextPos[dim_y] == map->pcPos[dim_y])
                || (t[i].trainerPos[dim_x] == map->pcPos[dim_x] && t[i].trainerPos[dim_y] == map->pcPos[dim_y])))
                {
                    t[i].reachedPC = 1;
                    pokemonBattle(allMaps[currentXmap][currentYmap], allArrays, 1, i);
                }
            }
            drawScreen(allMaps[currentXmap][currentYmap], currentXmap, currentYmap);
            refresh();
            if (allMaps[currentXmap][currentYmap]->justTerrainMap[allMaps[currentXmap][currentYmap]->pcPos[dim_y]][allMaps[currentXmap][currentYmap]->pcPos[dim_x]] == ':')
            {
                int chanceEncounter = rand() % 10;
                if (chanceEncounter == 0)
                {
                    pokemonBattle(allMaps[currentXmap][currentYmap], allArrays, false, 0);
                    drawScreen(allMaps[currentXmap][currentYmap], currentXmap, currentYmap);
                    refresh();
                }
            }
            if (pcDir == "qq")
            {
                quit_game = 1;
                endwin();
                return 0;
            }
        }
    }
    endwin();
    return 0;
}
