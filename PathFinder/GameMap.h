#pragma once

#include <string>
#include <vector>
#include "structures.h"
#include <queue>

class GameMap
{
	public:
		GameMap(int argc, char * argv);
		~GameMap();
		void DisplayMap();
		int CalcDistance(Coordinates current_point);
		void CalculatePathToTarget(Coordinates current_point);
		void OutputShortestPathToTarget();
		Coordinates GetNorthPoint(Coordinates input);
		Coordinates GetSouthPoint(Coordinates input);
		Coordinates GetEastPoint(Coordinates input);
		Coordinates GetWestPoint(Coordinates input);
		bool IsPointValid(Coordinates input);

	private:
		void GenerateRandomMap(int size);
		void LoadMap(std::string path_to_file);
		void InitGameMap();

		map_cell game_map[64][64]; //hard code allocation for now
		std::vector<queue_entry> * queue_list;

		// these variables are used in the CalculatePathToTarget recursive function
		// Moving these to member variables eliminates the risk of a stack overflow from recursion
		Coordinates invalid_cell;
		Coordinates point_north;
		Coordinates point_south;
		Coordinates point_east;
		Coordinates point_west;
		Coordinates previous_point;
		int distance_from_north;
		int distance_from_south;
		int distance_from_east;
		int distance_from_west;
		int local_weight;
		queue_entry add_this;

	public:
		//std::vector<std::tuple<Coordinates origin, Coordinates next, int distance>> path_list;
		Coordinates start_point;
		Coordinates target_point;
		Coordinates distance;
};
