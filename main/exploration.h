#pragma once
#ifndef MAZEEXP_H
#define MAZEEXP_H
#include "hardware.h"
#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
#include "Maze.h"
#define bluetooth Serial3

#include <StackList.h>

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
//void floodFillUpdate(coord currCoord, coord desired) {
//  StackList<coord> entries;
//
//  maze[currCoord.y][currCoord.x].walls = readCurrent();
//  entries.push(currCoord);
//
//  for (int i = 0; i < sizeof(headArray); i++) {
//    int dir = headArray[i];
//    //If there's a wall in this dir
//    if ((maze[currCoord.y][currCoord.x].walls & dir) == 0) {
//      coord workingCoord = { currCoord.x,currCoord.y };
//      switch (dir) {
//      case 1:
//        workingCoord.y = workingCoord.y - 1;
//        coordUpdate(workingCoord, 2);
//        break;
//      case 2:
//        workingCoord.y = workingCoord.y + 1;
//        coordUpdate(workingCoord, 1);
//        break;
//      case 4:
//        workingCoord.x = workingCoord.x + 1;
//        coordUpdate(workingCoord, 8);
//        break;
//      case 8:
//        workingCoord.x = workingCoord.x - 1;
//        coordUpdate(workingCoord, 4);
//        break;
//      }
//      //If the workingEntry is a valid entry and not a dead end, push it onto the stack
//      if (checkBounds(workingCoord) && (!isEnd(workingCoord, desired))) {
//        entries.push(workingCoord);
//      }
//    }
//  }
//  //While the entries stack isn't empty
//  while (!entries.isEmpty()) {
//    //Pop an entry from the stack
//    coord workingEntry = entries.pop();
//    int neighCheck = checkNeighs(workingEntry);
//    //If the least neighbor of the working entry is not one less than the value of the working entry
//    if (neighCheck + 1 != maze[workingEntry.y][workingEntry.x].distance) {
//      maze[workingEntry.y][workingEntry.x].distance = neighCheck + 1;
//      for (int i = 0; i < sizeof(headArray); i++) {
//        int dir = headArray[i];
//        if ((maze[workingEntry.y][workingEntry.x].walls & dir) != 0) {
//          coord nextCoord = bearingCoord(workingEntry, dir);
//          if (checkBounds(nextCoord)) {
//            if (!isEnd(nextCoord, desired)) {
//              entries.push(nextCoord);
//            }
//          }
//        }
//      }
//    }
//  }
//}

//void floodFill(coord desired[]) {
//  coord currCoord = { 0,0 };
//  int heading = 4;
//  /*Integer representation of heading
//  * 1 = N
//  * 4 = E
//  * 2 = S
//  * 8 = W
//  */
//  while (maze[currCoord.y][currCoord.x].distance != 0) {
//    floodFillUpdate(currCoord, desired);
//    int nextHeading = orient(currCoord, heading);
//    coord nextCoord = bearingCoord(currCoord, nextHeading);
//    //TODO: ADD MOVING INSTRUCTIONS HERE
//    
//    //This should occur as a callback of the moving finishing
//    currCoord = nextCoord;
//    heading = nextHeading;
//  }
//}
#endif
