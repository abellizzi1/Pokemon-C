#include <iostream>
#include <string>
#include "initialize_map.h"

void copy2dArrayToPointer(char map[21][80], struct Map *allMapsCurr[399][399], int currentXmap, int currentYmap)
{
    for (int r = 0; r < 21; r++)
    {
        for (int c = 0; c < 80; c++)
        {
            allMapsCurr[currentXmap][currentYmap]->built_map[r][c] = map[r][c];
            allMapsCurr[currentXmap][currentYmap]->justTerrainMap[r][c] = map[r][c];
        }
    }
}

char *stringToChar(std::string s)
{
    char *ret = new char[s.size() + 1];
    std::copy(s.begin(), s.end(), ret);
    ret[s.size()] = '\0';
    return ret;
}

int isValidPath(char c)
{
    if (c == ':' || c == '.')
    {
        return 1;
    }
    return 0;
}

void addTreesAndBoulders(char map[21][80])
{
    int numCompleted = 0;
    while (numCompleted < 40)
    {
        int randX = (rand() % 78) + 1;
        int randY = (rand() % 19) + 1;
        if (map[randY][randX] == ':' && numCompleted < 20)
        {
            map[randY][randX] = '%';
            numCompleted++;
        }
        else if (map[randY][randX] == ':')
        {
            map[randY][randX] = '"';
            numCompleted++;
        }
    }
}

void addClearings(char map[21][80])
{
    int leftClearingY = (rand() % 10) + 1;
    int rightClearingY = (rand() % 10) + 1;
    for (int r = leftClearingY; r < (leftClearingY+9); r++)
    {
        for (int c = 1; c < 21; c++)
        {
            map[r][c] = '.';
            if (c == 20 || r == leftClearingY || r == leftClearingY+8)
            {
                int halfChance = rand() % 2;
                if (halfChance == 0)
                {
                    map[r][c] = ':';
                }
            }
        }
    }
    for (int r = rightClearingY; r < (rightClearingY+9); r++)
    {
        for (int c = 78; c > 58; c--)
        {
            map[r][c] = '.';
            if (c == 59 || r == rightClearingY || r == rightClearingY+8)
            {
                int halfChance = rand() % 2;
                if (halfChance == 0)
                {
                    map[r][c] = ':';
                }
            }
        }
    }
}

void generatePaths(char map[21][80], struct Map *allMapsCurr[399][399], int currentXmap, int currentYmap)
{
    int topExit = (rand() % 78) + 1;
    int bottomExit = (rand() % 78) + 1;
    int leftExit = (rand() % 19) + 1;
    int rightExit = (rand() % 19) + 1;
    int randX = (rand() % 74) + 3;
    int randY = (rand() % 15) + 3;

    while (leftExit - rightExit >= -3 && leftExit - rightExit <= 3)
    {
        rightExit = (rand() % 19) + 1;
    }
    while (topExit - bottomExit >= -3 && topExit - bottomExit <= 3)
    {
        bottomExit = (rand() % 78) + 1;
    }
    while ((randY - leftExit >= -2 && randY - leftExit <= 2) || (randY - rightExit >= -2 && randY - rightExit <= 2))
    {
        randY = (rand() % 15) + 3;
    }
    while ((randX - topExit >= -2 && randX - topExit <= 2) || (randX - bottomExit >= -2 && randX - bottomExit <= 2))
    {
        randX = (rand() % 74) + 3;
    }

    if (currentXmap > 0 && allMapsCurr[currentXmap-1][currentYmap]->built_map[0][0] == '%')
    {
        leftExit = allMapsCurr[currentXmap-1][currentYmap]->right_exit;
    }
    if (currentXmap < 398 && allMapsCurr[currentXmap+1][currentYmap]->built_map[0][0] == '%')
    {
        rightExit = allMapsCurr[currentXmap+1][currentYmap]->left_exit;
    }
    if (currentYmap > 0 && allMapsCurr[currentXmap][currentYmap-1]->built_map[0][0] == '%')
    {
        bottomExit = allMapsCurr[currentXmap][currentYmap-1]->top_exit;
    }
    if (currentYmap < 398 && allMapsCurr[currentXmap][currentYmap+1]->built_map[0][0] == '%')
    {
        topExit = allMapsCurr[currentXmap][currentYmap+1]->bottom_exit;
    }
    allMapsCurr[currentXmap][currentYmap]->top_exit = topExit;
    allMapsCurr[currentXmap][currentYmap]->bottom_exit = bottomExit;
    allMapsCurr[currentXmap][currentYmap]->right_exit = rightExit;
    allMapsCurr[currentXmap][currentYmap]->left_exit = leftExit;

    //from the left
    for (int i = 0; i <= randX; i++)
    {
        map[leftExit][i] = '#';
    }
    //from the right
    for (int i = 79; i >= randX; i--)
    {
        map[rightExit][i] = '#';
    }
    //connect left and right
    if (leftExit > rightExit)
    {
        for (int i = rightExit; i <= leftExit; i++)
        {
            map[i][randX] = '#';
        }
    }
    else
    {
        for (int i = leftExit; i <= rightExit; i++)
        {
            map[i][randX] = '#';
        }
    }
    //from the bottom
    for (int i = 20; i >= randY; i--)
    {
        map[i][bottomExit] = '#';
    }
    //from the top
    for (int i = 0; i <= randY; i++)
    {
        map[i][topExit] = '#';
    }
    //connect top and bottom
    if (topExit > bottomExit)
    {
        for (int i = bottomExit; i <= topExit; i++)
        {
            map[randY][i] = '#';
        }
    }
    else
    {
        for (int i = topExit; i <= bottomExit; i++)
        {
            map[randY][i] = '#';
        }
    }

    if (currentXmap == 0)
    {
        map[leftExit][0] = '%';
    }
    if (currentXmap == 398)
    {
        map[rightExit][79] = '%';
    }
    if (currentYmap == 0)
    {
        map[20][bottomExit] = '%';
    }
    if (currentYmap == 398)
    {
        map[0][topExit] = '%';
    }

    float probabilityMartCenter;
    float randFloat1 = (float) rand() / RAND_MAX;
    float randFloat2 = (float) rand() / RAND_MAX;
    int mhDistance = abs(currentXmap-199) + abs(currentYmap-199);
    if (mhDistance >= 200)
    {
        probabilityMartCenter = 0.05;
    }
    else
    {
        probabilityMartCenter = (((-45 * (float) mhDistance)/200) + 50)/100;
    }

    if (randFloat1 <= probabilityMartCenter)
    {
        //PokeMart and PokeCenter
        if (leftExit > rightExit)
        {
            for (int i = rightExit; i <= leftExit; i++)
            {
                if (isValidPath(map[i][randX+1]) == 1 && isValidPath(map[i][randX+2]) == 1 && isValidPath(map[i+1][randX+1]) == 1 && isValidPath(map[i+1][randX+2]) == 1)
                {
                    map[i][randX+1] = 'C';
                    map[i][randX+2] = 'C';
                    map[i+1][randX+1] = 'C';
                    map[i+1][randX+2] = 'C';
                    break;
                }
                else if (isValidPath(map[i][randX-1]) == 1 && isValidPath(map[i][randX-2]) == 1 && isValidPath(map[i+1][randX-1]) == 1 && isValidPath(map[i+1][randX-2]) == 1)
                {
                    map[i][randX-1] = 'C';
                    map[i][randX-2] = 'C';
                    map[i+1][randX-1] = 'C';
                    map[i+1][randX-2] = 'C';
                    break;
                }
            }
        }
        else
        {
            for (int i = leftExit; i <= rightExit; i++)
            {
                if (isValidPath(map[i][randX+1]) == 1 && isValidPath(map[i][randX+2]) == 1 && isValidPath(map[i+1][randX+1]) == 1 && isValidPath(map[i+1][randX+2]) == 1)
                {
                    map[i][randX+1] = 'C';
                    map[i][randX+2] = 'C';
                    map[i+1][randX+1] = 'C';
                    map[i+1][randX+2] = 'C';
                    break;
                }
                else if (isValidPath(map[i][randX-1]) == 1 && isValidPath(map[i][randX-2]) == 1 && isValidPath(map[i+1][randX-1]) == 1 && isValidPath(map[i+1][randX-2]) == 1)
                {
                    map[i][randX-1] = 'C';
                    map[i][randX-2] = 'C';
                    map[i+1][randX-1] = 'C';
                    map[i+1][randX-2] = 'C';
                    break;
                }
            }
        }
    }

    if (randFloat2 <= probabilityMartCenter)
    {
        if (topExit > bottomExit)
        {
            for (int i = bottomExit; i <= topExit; i++)
            {
                if (isValidPath(map[randY+1][i]) == 1 && isValidPath(map[randY+2][i]) == 1 && isValidPath(map[randY+1][i+1]) == 1 && isValidPath(map[randY+2][i+1]) == 1)
                {
                    map[randY+1][i] = 'M';
                    map[randY+2][i] = 'M';
                    map[randY+1][i+1] = 'M';
                    map[randY+2][i+1] = 'M';
                    break;
                }
                else if (isValidPath(map[randY-1][i]) == 1 && isValidPath(map[randY-2][i]) == 1 && isValidPath(map[randY-1][i+1]) == 1 && isValidPath(map[randY-2][i+1]) == 1)
                {
                    map[randY-1][i] = 'M';
                    map[randY-2][i] = 'M';
                    map[randY-1][i+1] = 'M';
                    map[randY-2][i+1] = 'M';
                    break;
                }
            }
        }
        else
        {
            for (int i = topExit; i <= bottomExit; i++)
            {
                if (isValidPath(map[randY+1][i]) == 1 && isValidPath(map[randY+2][i]) == 1 && isValidPath(map[randY+1][i+1]) == 1 && isValidPath(map[randY+2][i+1]) == 1)
                {
                    map[randY+1][i] = 'M';
                    map[randY+2][i] = 'M';
                    map[randY+1][i+1] = 'M';
                    map[randY+2][i+1] = 'M';
                    break;
                }
                else if (isValidPath(map[randY-1][i]) == 1 && isValidPath(map[randY-2][i]) == 1 && isValidPath(map[randY-1][i+1]) == 1 && isValidPath(map[randY-2][i+1]) == 1)
                {
                    map[randY-1][i] = 'M';
                    map[randY-2][i] = 'M';
                    map[randY-1][i+1] = 'M';
                    map[randY-2][i+1] = 'M';
                    break;
                }
            }
        }
    }
    addTreesAndBoulders(map);
}

void generateMap(char map[21][80], struct Map *allMapsCurr[399][399], int currentXmap, int currentYmap)
{
    for (int r = 0; r < 21; r++)
    {
        for (int c = 0; c < 80; c++)
        {
            map[r][c] = ':';
        }
    }
    for (int i = 0; i < 80; i++)
    {
        map[0][i] = '%';
        map[20][i] = '%';
    }
    for (int i = 0; i < 21; i++)
    {
        map[i][0] = '%';
        map[i][79] = '%';
    }
    int mhDistance = abs(currentXmap-199) + abs(currentYmap-199);
    if (mhDistance <= 200)
    {
        allMapsCurr[currentXmap][currentYmap]->minPokemonLevel = 1;
        allMapsCurr[currentXmap][currentYmap]->maxPokemonLevel = mhDistance/2;
    }
    else
    {
        allMapsCurr[currentXmap][currentYmap]->minPokemonLevel = (mhDistance-200)/2;
        allMapsCurr[currentXmap][currentYmap]->maxPokemonLevel = 100;
    }
    addClearings(map);
    generatePaths(map, allMapsCurr, currentXmap, currentYmap);
}
