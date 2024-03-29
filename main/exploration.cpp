#include "exploration.h"
extern byte headArray[] = {8,4,2,1};//ESWN
extern MazeCell maze[Y][X];
extern coord curCoord;

//Get the most optimistic distance between two coordinates in a grid
int calcDist(int posx, int posy, int desireX, int desireY) {
	int dist = abs(desireY - posy) + abs(desireX - posx);
	return dist;
}

//Take a coordinate and test if it is within the allowable bounds
bool checkBounds(coord Coord) {
	if ((Coord.x >= X) || (Coord.y >= Y) || (Coord.x < 0) || (Coord.y < 0)) { return false; }
	else { return true; }
}

void instantiate() {
	for (int j = 0; j < Y; j++) {
		for (int i = 0; i < X; i++) {
			maze[j][i].distance = calcDist(i, j, destX, destY);
			maze[j][i].walls = 0;
			maze[j][i].marked = false;
			//If this is the left column (0,x)
			if (i == 0) {
				maze[j][i].walls = 2;
			}
			//if this is the top row
			if (j == 0) {
				maze[j][i].walls = 1;
			}
			//if this is the bottom row
			if (j == (Y - 1)) {
				maze[j][i].walls = 4;
			}
			//If this is the righ column
			if (i == (X - 1)) {
				maze[j][i].walls = 8;
			}
			maze[0][0].walls = 3;
			maze[Y - 1][0].walls = 6;
			maze[0][X - 1].walls = 9;
			maze[X - 1][Y - 1].walls = 12;
		}
	}
}


/*
INPUT: a coordinate representing a current position, and a heading
OUTPUT: the coordinates of the next desired position based on the heading and current position
*/
coord bearingCoord(coord currCoord, int heading) {
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

/*
INPUT: A Coord representing the current coordiante and the robots current heading
OUTPUT: An optimal direction away from the current coordinate.
*/
byte orient(coord currCoord, int heading) {

	//  coord leastNext = { 0,0 };
	  //This is the absolute largest value possible (dimension of maze squared)
	int leastNextVal = X * Y;
	int leastDir = heading;

	//If there is a bitwise equivalence between the current heading and the cell's value, then the next cell is accessible
  //  if ((maze[currCoord.y][currCoord.x].walls & heading) == 0) {
  //    //Define a coordinate for the next cell based onthis heading and set the leastNextVal t its value
  //    coord leastnextTemp = bearingCoord(currCoord, heading);
  //
  //    if (checkBounds(leastnextTemp)) {
  //      leastNext = leastnextTemp;
  //      leastNextVal = maze[leastNext.y][leastNext.x].distance;
  //    }
  //  }

	for (int i = 0; i < sizeof(headArray); i++) {
		int dir = headArray[i];
		//if this dir is accessible
		if ((maze[currCoord.y][currCoord.x].walls & dir) == 0) {
			//define the coordiante for this dir
			coord dirCoord = bearingCoord(currCoord, dir);

			if (checkBounds(dirCoord)) {
				//if this dir is more optimal than continuing straight
				if (maze[dirCoord.y][dirCoord.x].distance < leastNextVal) {
					//update teh value of leastNextVal
					leastNextVal = maze[dirCoord.y][dirCoord.x].distance;
					//update the value of leastnext to this dir
		  //          leastNext = dirCoord;
					leastDir = dir;
				}
			}
		}
	}
	return leastDir;
}



/*
INPUT: Coord
OUTPUT: An integer that is the least neighbor
*/
int checkNeighs(coord Coord) {
	int minVal = X * Y;
	for (int i = 0; i < sizeof(headArray); i++) {
		int dir = headArray[i];
		//if this dir is accessible
		if ((maze[Coord.y][Coord.x].walls & dir) == 0) {
			//Get the coordinate of the accessible neighbor
			coord neighCoord = bearingCoord(Coord, dir);
			//Check the value of the accessible neighbor
			if (checkBounds(neighCoord)) {
				//if the neighbore is less than the current recording minimum value, update the minimum value
				//If minVal is null, set it right away, otherwise test
				if (maze[neighCoord.y][neighCoord.x].distance < minVal) { minVal = maze[neighCoord.y][neighCoord.x].distance; }
			}
		}
	}
	return minVal;
}

//Given a coordinate, test and return if the coordinate is bounded on three sides
bool isDead(coord coord) {
	boolean deadEnd = false;
	if (checkBounds(coord)) {
		int bounds = maze[coord.y][coord.x].walls;
		//bounds is the integer from the exploratory maze that represents the known walls of the coordinate
		if ((bounds == 14) || (bounds == 13) || (bounds == 11) || (bounds == 7)) { deadEnd = true; }
	}
	return deadEnd;
}

bool isEnd(coord Coord) {
	boolean End = false;
	if (Coord.x == destX && Coord.y == destY) {
		End = true;
	}
	return End;
}

/*
This function makes calls to the dispatcher to get the following info
  -orientation
  -surrounding walls
Using orientation and walls, this information is mapped to a map integer in the global coordinate frame
*/
int readCurrent(coord coord, int heading, int *lfr) {
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


/*
INPUT: Coordindate to update, and a direction representing the wall to add
OUTPUT: Update to coordinate adding the wall provided as an argument
*/

void coordUpdate(coord coordinate, int wallDir) {
	if (checkBounds(coordinate)) {
		if ((maze[coordinate.y][coordinate.x].walls & wallDir) != 0) {
			maze[coordinate.y][coordinate.x].walls = maze[coordinate.y][coordinate.x].walls + wallDir;
		}
	}
}

void floodFillUpdate(coord currCoord, int heading, int *lfr) {
  StackList<coord> entries;

  maze[currCoord.y][currCoord.x].walls = readCurrent(currCoord,heading, lfr);
  bluetooth.print("Current walls: ");
  bluetooth.println(maze[currCoord.y][currCoord.x].walls);
  entries.push(currCoord);

  for (int i = 0; i < sizeof(headArray); i++) {
    int dir = headArray[i];
    //If there's a wall in this dir
    if ((maze[currCoord.y][currCoord.x].walls & dir) == 0) {
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
    if (neighCheck + 1 != maze[workingEntry.y][workingEntry.x].distance) {
      maze[workingEntry.y][workingEntry.x].distance = neighCheck + 1;
      for (int i = 0; i < sizeof(headArray); i++) {
        int dir = headArray[i];
        if ((maze[workingEntry.y][workingEntry.x].walls & dir) == 0) {
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

/*
INPUT: Current Robot coordinate
OUTPUT: Update maze for learned walls
*/
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
