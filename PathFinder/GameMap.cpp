#include "GameMap.h"
#include <iostream>
#include "structures.h"
#include <fstream>
#include <json/json.h>


// Construtcor allocates memory,
// initalizes the game map with values
// inspects launch-time parameters to see if a JSON map file has been provided, parses it and loads it into memory
// and randomly generates a game map if a JSON map file has not been provided
GameMap::GameMap(int argc, char* argv)
{
	invalid_cell = Coordinates{ -1, -1 };
	queue_list = new std::vector<queue_entry>;
	bool empty_queue = queue_list->empty();

	std::cout << "GameMap\n";
	InitGameMap();
	if (argc > 1)
	{
		std::cout << "use provided map\n";
		std::string path_to_file(argv); //error checking here
		LoadMap(path_to_file);
	}
	else
	{
		std::cout << "generate random map\n";
		int size = 64;
		GenerateRandomMap(size); //we can customize how big the random map should be
		std::cout << "Starting point: [" << start_point.row << ", " << start_point.col << "]\n";
		std::cout << "Target point: [" << target_point.row << ", " << target_point.col << "]\n";
	}
}

//Deconstructor deletes the queue_list data structure that was allocated in the constructor
GameMap::~GameMap()
{
	std::cout << "~GameMap\n";
	delete queue_list;
}

//DisplayMap sends the map to the standard output.  
//M is a "mountain", a cell that can not be traversed
//T is the target area
//B is the beginning or starting area
//" " is an empty space that is available for traversing
//"." is part of the path that is used to traverse from begining to end
void GameMap::DisplayMap()
{
	std::cout << "DisplayMap\n";
	std::cout << "+----------------------------------------------------------------+\n";
	for (int row = 0; row < 64; row++)
	{
		std::string display_line = "+";
		for (int col = 0; col < 64; col++)
		{
			switch (game_map[row][col].map_value)
			{
				case BLOCKED:
					display_line += "M";
					break;
				case TARGET:
					display_line += "T";
					break;
				case START:
					display_line += "B";
					break;
				case WALKED:
					display_line += ".";
					break;
				default:
					display_line += " ";
				break;
			}
		}
		std::cout << display_line + "+ " << row << "\n";
	}
	std::cout << "+----------------------------------------------------------------+\n";
}

//CalcDistance uses the "Manhatten" measurement to determine distance from a cell to the target
//X units over, Y units down
int GameMap::CalcDistance(Coordinates current_point)
{
	//"Manhatten" distance, regardless of direction
	return abs(current_point.row - target_point.row) + abs(current_point.col - target_point.col);
}

//CalculatePathToTarget accessing the points to the north, south, east and west.
//Evaluates the distance to target for all four and selects the closest one(s) for recursive processing
void GameMap::CalculatePathToTarget(Coordinates current_point)
{
	if ((current_point.row == target_point.row) && (current_point.col == target_point.col))
	{
		return;
	}
	
	game_map[current_point.row][current_point.col].cell_visited = true;
	local_weight = game_map[current_point.row][current_point.col].weight;
	point_north = GetNorthPoint(current_point);
	point_south = GetSouthPoint(current_point);
	point_east = GetEastPoint(current_point);
	point_west = GetWestPoint(current_point);
	
	distance_from_north = INT_MAX;
	distance_from_south = INT_MAX;
	distance_from_east = INT_MAX;
	distance_from_west = INT_MAX;

	if (IsPointValid(point_north))
	{
		distance_from_north = CalcDistance(point_north);
	}
	if (IsPointValid(point_south))
	{
		distance_from_south = CalcDistance(point_south);
	}
	if (IsPointValid(point_east))
	{
		distance_from_east = CalcDistance(point_east);
	}
	if (IsPointValid(point_west))
	{
		distance_from_west = CalcDistance(point_west);
	}

	if (distance_from_north < distance_from_south)
	{
		point_south = invalid_cell;
	}
	if (distance_from_north < distance_from_west)
	{
		point_west = invalid_cell;
	}
	if (distance_from_north < distance_from_east)
	{
		point_east = invalid_cell;
	}

	if (distance_from_south < distance_from_north)
	{
		point_north = invalid_cell;
	}
	if (distance_from_south < distance_from_west)
	{
		point_west = invalid_cell;
	}
	if (distance_from_south < distance_from_east)
	{
		point_east = invalid_cell;
	}

	if (distance_from_east < distance_from_north)
	{
		point_north = invalid_cell;
	}
	if (distance_from_east < distance_from_west)
	{
		point_west = invalid_cell;
	}
	if (distance_from_east < distance_from_south)
	{
		point_south = invalid_cell;
	}

	if (distance_from_west < distance_from_north)
	{
		point_north = invalid_cell;
	}
	if (distance_from_west < distance_from_east)
	{
		point_east = invalid_cell;
	}
	if (distance_from_west < distance_from_south)
	{
		point_south = invalid_cell;
	}

	if (IsPointValid(point_north))
	{
		game_map[point_north.row][point_north.col].weight = local_weight + 1;
	    add_this = { current_point, point_north, local_weight + 1 };
		queue_list->push_back(add_this);
	}
	if (IsPointValid(point_south))
	{
		game_map[point_south.row][point_south.col].weight = local_weight + 1;
		add_this = { current_point, point_south, local_weight + 1 };
		queue_list->push_back(add_this);
	}
	if (IsPointValid(point_east))
	{
		game_map[point_east.row][point_east.col].weight = local_weight + 1;
		add_this = { current_point, point_east, local_weight + 1 };
		queue_list->push_back(add_this);
	}
	if (IsPointValid(point_west))
	{
		game_map[point_west.row][point_west.col].weight = local_weight + 1;
		add_this = { current_point, point_west, local_weight + 1 };
		queue_list->push_back(add_this);
	}
	if (IsPointValid(point_north))
	{
		CalculatePathToTarget(point_north);
	}
	if (IsPointValid(point_south))
	{
		CalculatePathToTarget(point_south);
	}
	if (IsPointValid(point_east))
	{
		CalculatePathToTarget(point_east);
	}
	if (IsPointValid(point_west))
	{
		CalculatePathToTarget(point_west);
	}

	//If this is true, we've hit a "dead end" and need to rewind to the parent cell
	if (!(IsPointValid(point_north)) && !(IsPointValid(point_south)) &&
	    !(IsPointValid(point_east)) && !(IsPointValid(point_west)) )
	{
		add_this = queue_list->at(queue_list->size() - 1);
		queue_list->pop_back(); //remove this dead-end entry off the list
		previous_point = add_this.parent;
		CalculatePathToTarget(previous_point);
	}
}

//OutputShortestPathToTarget examines the created queue_list vector and reverse-traces
//the path from the target back to the start.
//There is a use-case scenario where the target is completely blocked, and no path is available.
//That scenario is identified and the proper output created.
void GameMap::OutputShortestPathToTarget()
{
	//target_point
	queue_entry queue_item;
	Coordinates search_entry = target_point;
	std::vector<Coordinates> path_list(0);

	//walk queue_list looking for entries with the child being the target_point.
	//Get the parent from that, and scan the entries again, repeating until each entry back 
	//to the beginning is found
	//add each entry to the path_list
	//iterate in reverse to output the walked path
	std::vector<queue_entry>::iterator queue_iter;
	bool path_scan_complete = false;
	queue_iter = queue_list->end();
	queue_iter--;
	while (!path_scan_complete)
	{
		queue_item = *queue_iter;
		
		if ((queue_item.child.row == search_entry.row) && (queue_item.child.col == search_entry.col))
		{
			path_list.push_back(queue_item.child);
			search_entry = queue_item.parent;
			queue_iter = queue_list->end();
			queue_iter--;
			if ((search_entry.row == start_point.row) && (search_entry.col == start_point.col))
			{
				path_list.push_back(search_entry);
				path_scan_complete = true;
			}
		}
		else
		{
			if (queue_iter == queue_list->begin()) //we've hit the begining of the list, stop searching
			{
				path_scan_complete = true;
			}
			else
			{
				queue_iter--;
			}
		}
	}
	std::vector<Coordinates>::reverse_iterator path_iter;
	std::cout << "PathFinder output:\n";
	if (path_list.size() == 0)
	{
		std::cout << "No path has been found.\n";
		//No path was found
	}
	else
	{
		for (path_iter = path_list.rbegin(); path_iter != path_list.rend(); ++path_iter)
		{
			search_entry = *path_iter;
			std::cout << "[" << search_entry.row << ", " << search_entry.col << "]\n";
			game_map[search_entry.row][search_entry.col].map_value = WALKED;
		}
		game_map[start_point.row][start_point.col].map_value = START;
		game_map[target_point.row][target_point.col].map_value = TARGET;
	}

}

//GetNorthPoint identifies the valid coordinates of the cell to the north of the input cell
//A default invalid cell will be returned if:
//1) The input cell is at the edge of the map
//2) The adjacent cell has already been visited (this prevents infinite loops)
//3) The adjacent cell is a "BLOCKED" cell (aka a wall/mountain/terrain)
Coordinates GameMap::GetNorthPoint(Coordinates input)
{
	Coordinates return_value{ -1,-1 };
	if (input.row == 0) //we are at the top of the map
	{
		return return_value;
	}
	if (game_map[input.row - 1][input.col].cell_visited == true)
	{
		return return_value;
	}
	// some compilers will stop evaluating a boolean once the first "false" is found, some may not.
	// to prevent an array out-of-bounds error, separate the "is this the edge of the map"
	// question from "what is the terrain type of an adjacent square" into two separate questions.
	if (game_map[input.row - 1][input.col].map_value == BLOCKED) //there is terrain
	{
		return return_value;
	}
	return_value.row = input.row - 1;
	return_value.col = input.col;
	return return_value;
}

//GetSouthPoint identifies the valid coordinates of the cell to the south of the input cell
//A default invalid cell will be returned if:
//1) The input cell is at the edge of the map
//2) The adjacent cell has already been visited (this prevents infinite loops)
//3) The adjacent cell is a "BLOCKED" cell (aka a wall/mountain/terrain)
Coordinates GameMap::GetSouthPoint(Coordinates input)
{
	Coordinates return_value{ -1,-1 };
	if (input.row == 63) //we are at the bottom of the map
	{
		return return_value;
	}
	if (game_map[input.row + 1][input.col].cell_visited == true)
	{
		return return_value;
	}
	// some compilers will stop evaluating a boolean once the first "false" is found, some may not.
	// to prevent an array out-of-bounds error, separate the "is this the edge of the map"
	// question from "what is the terrain type of an adjacent square" into two separate questions.
	if (game_map[input.row + 1][input.col].map_value == BLOCKED) //there is terrain
	{
		return return_value;
	}
	return_value.row = input.row + 1;
	return_value.col = input.col;
	return return_value;
}

//GetEastPoint identifies the valid coordinates of the cell to the east of the input cell
//A default invalid cell will be returned if:
//1) The input cell is at the edge of the map
//2) The adjacent cell has already been visited (this prevents infinite loops)
//3) The adjacent cell is a "BLOCKED" cell (aka a wall/mountain/terrain)
Coordinates GameMap::GetEastPoint(Coordinates input)
{
	Coordinates return_value{ -1,-1 };
	if (input.col == 63) //we are at the eastern edge of the map
	{
		return return_value;
	}
	if (game_map[input.row][input.col + 1].cell_visited == true)
	{
		return return_value;
	}
	// some compilers will stop evaluating a boolean once the first "false" is found, some may not.
	// to prevent an array out-of-bounds error, separate the "is this the edge of the map"
	// question from "what is the terrain type of an adjacent square" into two separate questions.
	if (game_map[input.row][input.col + 1].map_value == BLOCKED) //there is terrain
	{
		return return_value;
	}
	return_value.row = input.row;
	return_value.col = input.col + 1;
	return return_value;
}

//GetWestPoint identifies the valid coordinates of the cell to the west of the input cell
//A default invalid cell will be returned if:
//1) The input cell is at the edge of the map
//2) The adjacent cell has already been visited (this prevents infinite loops)
//3) The adjacent cell is a "BLOCKED" cell (aka a wall/mountain/terrain)
Coordinates GameMap::GetWestPoint(Coordinates input)
{
	Coordinates return_value{ -1,-1 };
	if (input.col == 0) //we are at the western edge of the map
	{
		return return_value;
	}
	if (game_map[input.row][input.col - 1].cell_visited == true)
	{
		return return_value;
	}
	// some compilers will stop evaluating a boolean once the first "false" is found, some may not.
	// to prevent an array out-of-bounds error, separate the "is this the edge of the map"
	// question from "what is the terrain type of an adjacent square" into two separate questions.
	if (game_map[input.row][input.col - 1].map_value == BLOCKED) //there is terrain
	{
		return return_value;
	}
	return_value.row = input.row;
	return_value.col = input.col -1;
	return return_value;
}

//GenerateRandomMap will, in the abcense of a launch-time parameter JSON map file,
//use the same 64x64 grid in the requirements and randomly generate terrain blocks 
//and a random start and target point
//Future work: allow for a default config value for the percentage of the grid that should
//be randomly filled with terrain, with a launch parameter that allows for modifying this percentage value
void GameMap::GenerateRandomMap(int size)
{
	std::cout << "GenerateRandomMap\n";
	srand((unsigned)time(NULL));

	short row = 0;
	short col = 0;

	int random_percentage = 25; //this could be a configurable value
	int block_count = size * size * random_percentage / 100;

	for (int loop_index = 0; loop_index < block_count; loop_index++)
	{
		row = (rand() % size);
		col = (rand() % size);
		game_map[row][col].map_value = 4; //set to elevated terrain
	}

	row = (rand() % size);
	col = (rand() % size);
	//row = 5;
	//col = 0;
	game_map[row][col].map_value = 0; //starting position
	start_point = { row, col };

	row = (rand() % size);
	col = (rand() % size);
	//row = 5;
	//col = 9;
	game_map[row][col].map_value = 6; //target position
	target_point = { row, col };
}

//Leveraging the JSON library loaded separately, read in the JSON file at the path provided.
// Process the second "name" (world) section and load the "data" section into the map_cell.
void GameMap::LoadMap(std::string path_to_file)
{
	std::cout << "LoadMap\n";
	Json::Value root;
	// Get the value of the member of root named 'my-encoding', return 'UTF-32' if there is no
	// such member.
	std::string my_encoding = root.get("my-encoding", "UTF-32").asString();

	std::ifstream input_doc(path_to_file, std::ifstream::binary);

	Json::CharReaderBuilder rbuilder;
	// Configure the Builder, then ...
	std::string errs;
	bool parsingSuccessful = Json::parseFromStream(rbuilder, input_doc, &root, &errs);
	if (!parsingSuccessful)
	{
		// report to the user the failure and their locations in the document.
		std::cout << "Failed to parse configuration\n"
			<< errs;
		return;
	}

	// Get the value of the member of root named 'layers'; return a 'null' value if
	// there is no such member.
	const Json::Value layers = root["layers"];
	int layers_count = layers.size();
	std::cout << "Layers count: " << layers_count << "\n";

	const Json::Value::ArrayIndex array_index = layers_count - 1; //0 based index
	const Json::Value world_data = layers.get(array_index, NULL);
	const Json::Value world_data_list = world_data["data"];

	int row = 0;
	int col = 0;
	for (int index = 0; index < (int)(world_data_list.size()); ++index)
	{
		int map_value = world_data_list[index].asInt();
		game_map[row][col].map_value = map_value;

		if (map_value == START)
		{
			start_point.row = row;
			start_point.col = col;
		}
		else if (map_value == TARGET)
		{
			target_point.row = row;
			target_point.col = col;
		}

		col++;
		if (col == 64)
		{
			col = 0;
			row++;
		}
	}
}

//InitGameMap intializes the game map prior to loading a JSON file or randomly generating a map
void GameMap::InitGameMap()
{
	for (int row = 0; row < 64; row++)
	{
		for (int col = 0; col < 64; col++)
		{
			game_map[row][col].map_value = -1;
			game_map[row][col].cell_visited = false;
			game_map[row][col].weight = 0; //default value
		}
	}
}

//IsPointValid returns a true/false depending on whether a supplied coordinate
//is "valid" (both values must be larger than -1.
//-1 is used to flag a coordinate as invalid, and should be skipped for processing
//Examples of an invalid coordinate would be a terrain cell, or attempting to read off the edge of the map
bool GameMap::IsPointValid(Coordinates input)
{
	return ((input.row != -1) && (input.col != -1));
}