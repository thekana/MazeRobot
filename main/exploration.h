#pragma once
#ifndef MAZEEXP_H
#define MAZEEXP_H
#include "hardware.h"
#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
#include "Maze.h"
#include "cppQueue.h"
#define bluetooth Serial3

#include "StackList.h"

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include "external_MPU6050_I2Cdev.h"
#include "hardware.h"
#include "hardware_definition.h"

#include "external_MPU6050_6Axis_MotionApps20.h"


struct coord {
	int x;
	int y;
};

struct MazeCell {
	int distance;
	byte walls;
	bool marked;
};

#define X 9
#define Y 5
#define destX 4
#define destY 2

//Get the most optimistic distance between two coordinates in a grid
int calcDist(int posx, int posy, int desireX, int desireY);

//Take a coordinate and test if it is within the allowable bounds
bool checkBounds(coord Coord);

void instantiate();


/*
INPUT: a coordinate representing a current position, and a heading
OUTPUT: the coordinates of the next desired position based on the heading and current position
*/
coord bearingCoord(coord currCoord, int heading) ;

/*
INPUT: A Coord representing the current coordiante and the robots current heading
OUTPUT: An optimal direction away from the current coordinate.
*/
byte orient(coord currCoord, int heading);

/*
INPUT: Coord
OUTPUT: An integer that is the least neighbor
*/
int checkNeighs(coord Coord);

//Given a coordinate, test and return if the coordinate is bounded on three sides
bool isDead(coord coord);

bool isEnd(coord Coord);

/*
This function makes calls to the dispatcher to get the following info
  -orientation
  -surrounding walls
Using orientation and walls, this information is mapped to a map integer in the global coordinate frame
*/
int readCurrent(coord coord,int heading,int *lfr);

/*
INPUT: Coordindate to update, and a direction representing the wall to add
OUTPUT: Update to coordinate adding the wall provided as an argument
*/

void coordUpdate(coord coordinate, int wallDir);

/*
INPUT: Current Robot coordinate
OUTPUT: Update maze for learned walls
*/
void floodFillUpdate(coord currCoord, int heading, int *lfr);

void exploration(Queue& motion_queue, int *lfr, int *startStep, String keyword, int *cellCount, int *heading);

#endif
