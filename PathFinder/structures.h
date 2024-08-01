#pragma once

// Coordinates serves to identify a location on a 2D grid
typedef struct
{
	short row;
	short col;
} Coordinates;

// map_cell allows for the storage of all the signficant details for a given cell
// map_value: a value designating a "terrain type" i.e. BLOCKED, OPEN, START, TARGET
typedef struct
{
	short map_value;
	bool cell_visited;
	int weight;
} map_cell;

// queue_entry serves as a data structure for the end result queue from the start point
// to the target point
typedef struct
{
	Coordinates parent;
	Coordinates child;
	int weight;		
} queue_entry;

// terrain_type presents an enum to be used for the map_value in map_cell
// adding WALKED as an option allows the application to show the final calculated result
enum terrain_type {
	OPEN = -1,
	START = 0,
	BLOCKED = 4,
	TARGET = 6,
	WALKED = 1
};