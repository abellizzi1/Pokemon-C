#include <iostream>
#include <string>
#include <curses.h>
#include <limits.h>

#include "draw_windows.h"

int findColorPair (char c)
{
    switch (c)
    {
        case ':':
            return 1;
        case '%':
        case '"':
            return 2;
        case 'M':
        case 'C':
            return 3;
        case 'r':
        case 'h':
        case 'p':
        case 'n':
        case 'w':
        case 's':
            return 4;
        case '#':
            return 5;
        case '@':
            return 7;
        default:
            return 6;
    }
}

void drawTrainersWindow(int startIdx, trainer_t trainers[], int amountOfTrainers, pair_t pcPos)
{
    WINDOW *listTrainersWin = newwin(21, 80, 1, 0);
    mvwprintw(listTrainersWin, 1, 35, "List of Trainers");
    mvwprintw(listTrainersWin, 2, 36, "Exit with ESC");
    int cursorPos = 3;
    for (int i = startIdx; i < amountOfTrainers && i < (startIdx + 18); i++)
    {
        int difNorth = trainers[i].trainerPos[dim_y] - pcPos[dim_y];
        int difEast = trainers[i].trainerPos[dim_x] - pcPos[dim_x];
        wmove(listTrainersWin, cursorPos, 32);
        if (difNorth < 0)
        {
            difNorth = abs(difNorth);
            wprintw(listTrainersWin, "%c, %d north and ", trainers[i].type, difNorth);
        }
        else
        {
            wprintw(listTrainersWin, "%c, %d south and ", trainers[i].type, difNorth);
        }
        if (difEast < 0)
        {
            difEast = abs(difEast);
            wprintw(listTrainersWin, "%d west", difEast);
        }
        else
        {
            wprintw(listTrainersWin, "%d east", difEast);
        }
        cursorPos++;
    }
    wrefresh(listTrainersWin);
}

void drawCenterWindow(int startIdx, struct Map *map, WINDOW *centerMartWin)
{
    wclear(centerMartWin);
    mvwprintw(centerMartWin, 0, 35, "PokeCenter");
    mvwprintw(centerMartWin, 3, 30, "All Pokemon Healed!");
    mvwprintw(centerMartWin, 5, 10, "Use the characters in parenthesis to move Pokemon to and from storage.");
    mvwprintw(centerMartWin, 6, 14, "Use UP and DOWN arrows to scroll through storage list.");
    mvwprintw(centerMartWin, 1, 34, "Exit with <");
    std::string pokePrint = "(a) " + map->trainers[0].pokemonList[0].name;
    mvwprintw(centerMartWin, 8, 1, stringToChar(pokePrint));
    if (map->trainers[0].numPokemon >= 2)
    {
        pokePrint = "(b) " + map->trainers[0].pokemonList[1].name;
        mvwprintw(centerMartWin, 9, 1, stringToChar(pokePrint));
    }
    if (map->trainers[0].numPokemon >= 3)
    {
        pokePrint = "(c) " + map->trainers[0].pokemonList[2].name;
        mvwprintw(centerMartWin, 10, 1, stringToChar(pokePrint));
    }
    if (map->trainers[0].numPokemon >= 4)
    {
        pokePrint = "(d) " + map->trainers[0].pokemonList[3].name;
        mvwprintw(centerMartWin, 11, 1, stringToChar(pokePrint));
    }
    if (map->trainers[0].numPokemon >= 5)
    {
        pokePrint = "(e) " + map->trainers[0].pokemonList[4].name;
        mvwprintw(centerMartWin, 12, 1, stringToChar(pokePrint));
    }
    if (map->trainers[0].numPokemon == 6)
    {
        pokePrint = "(f) " + map->trainers[0].pokemonList[5].name;
        mvwprintw(centerMartWin, 13, 1, stringToChar(pokePrint));
    }

    int cursorPos = 8;
    int inputNum = 1;
    for (int i = startIdx; i < map->numPokemonStorage && i < (startIdx + 13) && inputNum < 10; i++)
    {
        std::string printStr = "(" + std::to_string(inputNum) + ") " + map->pokeStorage[i].name;
        mvwprintw(centerMartWin, cursorPos, 50, stringToChar(printStr));
        cursorPos++;
        inputNum++;
    }
}

void drawScreen(struct Map *map, int currentXmap, int currentYmap)
{
    int currColorPair = 1;
    move(1, 0);
    for (int r = 0; r < 21; r++)
    {
        for (int c = 0; c < 80; c++)
        {
            currColorPair = findColorPair(map->built_map[r][c]);
            attron(COLOR_PAIR(currColorPair));
            printw("%c", map->built_map[r][c]);
            attroff(COLOR_PAIR(currColorPair));
        }
        printw("\n");
    }
    refresh();
    printw("(%d, %d)\n", (currentXmap-199), (currentYmap-199));
}
