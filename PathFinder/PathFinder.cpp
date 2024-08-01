// PathFinder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GameMap.h"

int main(int argc, char* argv[])
{
    std::cout << "Hello World!\n";

    //determine if map file is provided.
    //if map file is provided, load it into memory
    //if map file is not provided, randomly generate a map file
    GameMap gameMap = GameMap(argc, argv[1]);

    gameMap.DisplayMap();

    //starting at the start_point, run a modifed A* algorithm to create a queue of parent->child cells
    //until the target point is located
    //one of the requirements is to allow for backtracking.
    gameMap.CalculatePathToTarget(gameMap.start_point);

    //once the target point is found, work backwards from the target, creating the short list
    //of cells visited from start to finish
    gameMap.OutputShortestPathToTarget();

    //produce output to screen for feedback
    gameMap.DisplayMap();

    return 0;
}


