# Pokemon-C
A Pokemon game written in C and C++ that is played in the terminal.

## How To Play
**(1)** Download all files from this repository (including pokedex folder)

**(2)** Open pokemon.cpp file and set the variable "dir" to the path to the pokedex folder in your directory (Line 1437)

**(3)** Open linux terminal and cd into the directory

**(4)** Run command "make"

**(5)** Run command "./pokemon"

**(6) *Optional*** Set number of trainers and rivals with "./pokemon --numtrainers [num]". Example: ./pokemon --numtrainers 5

## Key Bindings, Map, and Trainer Information
### Key Bindings
**7 or y** Move player (represented by @) upper left

**8 or k** Move player up

**9 or u** Move player upper right

**6 or l** Move player right

**3 or n** Move player lower right

**2 or j** Move player down

**1 or b** Move player lower left

**4 or h** Move player left

**>** While standing on a PokeMart (M) or PokeCenter (C), enter the building. Exit with <

**5 or space or .** Rest for a turn. Trainers still move

**t** Display list of trainers with the position relative to the player

**up arrow** Scroll trainers list up (while trainers list is displayed)

**down arrow** Scroll trainers list down (while trainers list is displayed)

**esc** Exit trainers list

**B (capital)** Access the player's bag

**Q (capital)** Quit game

### Map Information
There are 399x399 randomly generated maps. Each map is 80x21 characters. To go to the next map, travel to the end of a path on any border of the map.

**%** Boulder (cannot step over boulders)

**#** Path (no chance of wild pokemon encounter)

**M** PokeMart (use > to enter, < to exit)

**C** PokeCenter (use > to enter, < to exit)

**:** Tall Grass (chance of wild pokemon encounter)

**.** Clearing (cannot step over clearings)

**"** Tree (cannot step over trees)

### Trainer Information
If any trainer reaches the player, a pokemon battle is triggered. Each trainer moves differently (except for hikers and rivals).

**@** Player

**h** Hiker: They path to the player using dijkstra's algorithm

**r** Rival: They path to the player using dijkstra's algorithm

**p** Pacer: They start with a direction and walk until they hit a terrain they cannot step over, then turn arround and repeat

**w** Wanderer: They do not leave the terrain that they were spawned on. They walk in a random direction until they hit the edge of the terrain, then turn arround and continue

**s** Stationary: They don't move

**n** Random walker: They move similar to a pacer, but they can cross any terrain
