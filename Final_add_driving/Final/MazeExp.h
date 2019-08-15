#ifndef MAZEEXP_H
#define MAZEEXP_H
#include "hardware.h"
#include "external_VL6180X.h"
#include "external_I2CIO.h"
#include "Wire.h"
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
  bool detected;
};

class MazeExp
{
private:
	byte X = 9;
	byte Y = 9;
	byte destX;
	byte destY;

public:
  MazeCell maze[9][9];
  byte headArray[4] = {8,4,2,1};//ESWN

public:
  MazeExp() {
//    instantiate();
  }

  byte getX() {
    return this->X;
  }
    
  byte getY() {
    return this->Y;
  }
  
  byte getDestX() {
    return this->destX;
  }
  
  byte getDestY() {
    return this->destY;
  }

  void setDestY(byte y){
    this->destY = y;  
  }

  void setDestX(byte x){
    this->destX = x;  
  }

  void setX(byte x){
    this->X = x;  
  }

  void setY(byte y){
    this->Y = y;  
  }
  
  byte calcDist(int posx, int posy, byte desireX, byte desireY) {
    int dist = abs(desireY - posy) + abs(desireX - posx);
    return dist;
  }
  
  bool checkBounds(coord Coord) {
    if ((Coord.x >= this->X) || (Coord.y >= this->Y) || (Coord.x < 0) || (Coord.y < 0)) { return false; }
    else { return true; }
  }
  
  void instantiate() {
    X = getX();
    Y = getY();
    for (int j = 0; j < Y; j++) {
      for (int i = 0; i < X; i++) {
        this->maze[j][i].distance = calcDist(i, j, this->getDestX(), this->getDestY());
        this->maze[j][i].walls = 0;
        this->maze[j][i].detected = false;
        //If this is the left column (0,x)
        if (i == 0) {
          this->maze[j][i].walls = 2;
        }
        //if this is the top row
        if (j == 0) {
          this->maze[j][i].walls = 1;
        }
        //if this is the bottom row
        if (j == (Y - 1)) {
          this->maze[j][i].walls = 4;
        }
        //If this is the righ column
        if (i == (X - 1)) {
          this->maze[j][i].walls = 8;
        }
        this->maze[0][0].walls = 3;
        this->maze[Y - 1][0].walls = 6;
        this->maze[0][X - 1].walls = 9;
        this->maze[X - 1][Y - 1].walls = 12;
      }
    }
  }

  void reloadDist() {
    X = getX();
    Y = getY();
    for (int j = 0; j < Y; j++) {
      for (int i = 0; i < X; i++) {
        this->maze[j][i].distance = calcDist(i, j, this->getDestX(), this->getDestY());
      }
    }
  }
  
  coord bearingCoord(coord currCoord, byte heading) {
    coord nextCoord = { 0,0 };
    switch (heading) {
    case 1:
      //code
      nextCoord.x = currCoord.x;
      nextCoord.y = currCoord.y - 1;
      break;
    case 4:
      nextCoord.x = currCoord.x;
      nextCoord.y = currCoord.y + 1;
      break;
    case 8:
      nextCoord.x = currCoord.x + 1;
      nextCoord.y = currCoord.y;
      break;
    case 2:
      nextCoord.x = currCoord.x - 1;
      nextCoord.y = currCoord.y;
      break;
    }
    return nextCoord;
  }
  
  byte orient(coord currCoord, byte heading) {
  
    X = getX();
    Y = getY();
    int leastNextVal = X * Y;
    int leastDir = heading;
  
  
    for (int i = 0; i < sizeof(this->headArray); i++) {
      int dir = this->headArray[i];
      //if this dir is accessible
      if ((this->maze[currCoord.y][currCoord.x].walls & dir) == 0) {
        //define the coordiante for this dir
        coord dirCoord = bearingCoord(currCoord, dir);
  
        if (checkBounds(dirCoord)) {
          //if this dir is more optimal than continuing straight
          if (this->maze[dirCoord.y][dirCoord.x].distance < leastNextVal) {
            //update teh value of leastNextVal
            leastNextVal = this->maze[dirCoord.y][dirCoord.x].distance;
            //update the value of leastnext to this dir
        //          leastNext = dirCoord;
            leastDir = dir;
          }
        }
      }
    }
    return leastDir;
  }
  
  int checkNeighs(coord Coord) {
    X = getX();
    Y = getY();
    int minVal = X * Y;
    for (int i = 0; i < sizeof(this->headArray); i++) {
      int dir = this->headArray[i];
      //if this dir is accessible
      if ((this->maze[Coord.y][Coord.x].walls & dir) == 0) {
        //Get the coordinate of the accessible neighbor
        coord neighCoord = bearingCoord(Coord, dir);
        //Check the value of the accessible neighbor
        if (checkBounds(neighCoord)) {
          //if the neighbore is less than the current recording minimum value, update the minimum value
          //If minVal is null, set it right away, otherwise test
          if (this->maze[neighCoord.y][neighCoord.x].distance < minVal) { minVal = this->maze[neighCoord.y][neighCoord.x].distance; }
        }
      }
    }
    return minVal;
  }
  
  bool isDead(coord coord) {
    boolean deadEnd = false;
    if (checkBounds(coord)) {
      int bounds = this->maze[coord.y][coord.x].walls;
      //bounds is the integer from the exploratory maze that represents the known walls of the coordinate
      if ((bounds == 14) || (bounds == 13) || (bounds == 11) || (bounds == 7)) { deadEnd = true; }
    }
    return deadEnd;
  }
  
  bool isEnd(coord Coord) {
    boolean End = false;
    if (Coord.x == getDestX() && Coord.y == getDestY()) {
      End = true;
    }
    return End;
  }
  
  byte readCurrent(coord coord, byte heading, int *lfr) {
    //TODO: Fill this bitch out
    int wallVal = 0;
  
    switch (heading) {
    case 8://face East
      wallVal = 8 * lfr[1];
      wallVal += 4 * lfr[2];
      wallVal += 1 * lfr[0];
      wallVal += 2 & maze[coord.y][coord.x].walls;
      break;
    case 4://face south
      wallVal = 8 * lfr[0];
      wallVal += 4 * lfr[1];
      wallVal += 2 * lfr[2];
      wallVal += 1 & maze[coord.y][coord.x].walls;
      break;
    case 2://face west
      wallVal = 4 * lfr[0];
      wallVal += 2 * lfr[1];
      wallVal += 1 * lfr[2];
      wallVal += 8 & maze[coord.y][coord.x].walls;
      break;
    case 1://face north
      wallVal = 1 * lfr[1];
      wallVal += 8 * lfr[2];
      wallVal += 2 * lfr[0];
      wallVal += 4 & maze[coord.y][coord.x].walls;
      break;
    }
    bluetooth.print("WallVal: "); bluetooth.println(wallVal);
    return wallVal;
  }
  
  void coordUpdate(coord coordinate, int wallDir) {
    if (checkBounds(coordinate)) {
      if ((this->maze[coordinate.y][coordinate.x].walls & wallDir) != 0) {
        this->maze[coordinate.y][coordinate.x].walls = this->maze[coordinate.y][coordinate.x].walls + wallDir;
      }
    }
  }
  
  void floodFillUpdate(coord currCoord, byte heading, int *lfr) {
    StackList<coord> entries;
  
    this->maze[currCoord.y][currCoord.x].walls = readCurrent(currCoord, heading, lfr);
//    bluetooth.print("Current walls: ");
//    bluetooth.println(this->maze[currCoord.y][currCoord.x].walls);
    entries.push(currCoord);
  
    for (int i = 0; i < sizeof(this->headArray); i++) {
      int dir = this->headArray[i];
      //If there's a wall in this dir
      if ((this->maze[currCoord.y][currCoord.x].walls & dir) == 0) {
        coord workingCoord = { currCoord.x,currCoord.y };
        switch (dir) {
        case 1:
          workingCoord.y = workingCoord.y - 1;
          coordUpdate(workingCoord, 4);
          break;
        case 4:
          workingCoord.y = workingCoord.y + 1;
          coordUpdate(workingCoord, 1);
          break;
        case 8:
          workingCoord.x = workingCoord.x + 1;
          coordUpdate(workingCoord, 2);
          break;
        case 2:
          workingCoord.x = workingCoord.x - 1;
          coordUpdate(workingCoord, 8);
          break;
        }
        //If the workingEntry is a valid entry and not a dead end, push it onto the stack
        if (checkBounds(workingCoord) && (!isEnd(workingCoord))) {
          entries.push(workingCoord);
        }
      }
    }
    //While the entries stack isn't empty
    while (!entries.isEmpty()) {
      //Pop an entry from the stack
      coord workingEntry = entries.pop();
      int neighCheck = checkNeighs(workingEntry);
      //If the least neighbor of the working entry is not one less than the value of the working entry
      if (neighCheck + 1 != this->maze[workingEntry.y][workingEntry.x].distance) {
        this->maze[workingEntry.y][workingEntry.x].distance = neighCheck + 1;
        for (int i = 0; i < sizeof(headArray); i++) {
          int dir = headArray[i];
          if ((this->maze[workingEntry.y][workingEntry.x].walls & dir) == 0) {
            coord nextCoord = bearingCoord(workingEntry, dir);
            if (checkBounds(nextCoord)) {
              if (!isEnd(nextCoord)) {
                entries.push(nextCoord);
              }
            }
          }
        }
      }
    }
  }
};
#endif
