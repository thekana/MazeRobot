#include "exploration.h"
#include "locomotion.h"
byte headArray[] = {8,4,2,1};//ESWN
extern MazeCell maze[Y][X];
extern coord curCoord;
extern Maze *mazePrint;

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

void exploration(Queue& motion_queue, int *lfr, int *startStep, String keyword, int *cellCount, byte *heading)
{
	int motion_decision = 0;
	if(*startStep == 2)
    {
        if(keyword.equals("s\n"))
        {
            if(lfr[2] == 0){
                curCoord.x = X - 1;
                *startStep = 3;
            }else if(lfr[0]==1){
            	//move forward one cell
            	motion_decision = MOTION_FORWARD;
                (*cellCount)++;                
                bluetooth.print("CellCount: "); bluetooth.println(*cellCount);
               	
            }else{
                *startStep = 3;
            }
        }
        else if(keyword.equals("l\n"))
        {
            if(lfr[0]==0){
                curCoord.x = X-1;
                *heading = 2;
                *startStep = 3;
            }else if(lfr[2]==1){
            	// move forward one cell
            	motion_decision = MOTION_FORWARD;
                (*cellCount)++;
                bluetooth.print("CellCount: "); bluetooth.println(*cellCount);
              
            }else{
                *heading = 8;
                *startStep = 3;
            }
        }
    }
    else if(*startStep == 3)
    {
        if(keyword.equals("s\n"))
        {
            if(*cellCount!=0)
            {
                curCoord.y = *cellCount;
                for(int i=0;i<*cellCount;i++)
                {
                    maze[i][curCoord.x].walls = 11;
                }
                *cellCount = 0;
    //        keyword = "";
            }
        }
        else if(keyword.equals("l\n"))
        {
            if(*cellCount!=0)
            {
                if(*heading==8)
                {
                    curCoord.x = *cellCount;
        //          maze[curCoord.y][0].walls = 4;
                    for(int i=1;i<*cellCount;i++)
                    {
                        maze[curCoord.y][i].walls = 7;
                    }
                }
                else if(*heading == 2)
                {
                    curCoord.x = X - 1 - (*cellCount);
        //          maze[curCoord.y][X-1].walls = 8;
                    for(int i=cellCount-1;i>=0;i--)
                    {
                        maze[curCoord.y][X-i].walls = 13;
                    }
                }
    //          maze[0][curCoord.x].walls = 2;
                *cellCount = 0;
            }
        }
         if(maze[curCoord.y][curCoord.x].distance != 0){
            bluetooth.print("Current cell distance: ");  bluetooth.println(maze[curCoord.y][curCoord.x].distance);  
  
            bluetooth.print("Wall: ");bluetooth.print(lfr[0]);bluetooth.print(lfr[1]);bluetooth.println(lfr[2]);
            bluetooth.print("Current cell: [");bluetooth.print(curCoord.y);
            bluetooth.print(" "); bluetooth.print(curCoord.x);bluetooth.println("] ");
            mazePrint->fillCells(curCoord.y,curCoord.x,maze[curCoord.y][curCoord.x].walls);
            int nextHeading = orient(curCoord, (*heading));
            coord nextCoord = bearingCoord(curCoord, nextHeading);
            if(*heading == nextHeading){
              //Move forward one cell
                motion_decision = MOTION_FORWARD;
            }else{
              switch (*heading) {
                case 1: //N
                  if(nextHeading==2){
                      motion_decision = MOTION_LEFT;
                  }else if(nextHeading == 4){
                      motion_decision = MOTION_BACK;  
                  }else if(nextHeading == 8){
                      motion_decision = MOTION_RIGHT;
                  }
                  break; 
                case 4: //S
                  if(nextHeading==2){
                      motion_decision = MOTION_RIGHT;
                  }else if(nextHeading == 8){
                      motion_decision = MOTION_LEFT;  
                  }else if(nextHeading == 1){
                      motion_decision = MOTION_BACK;
                  }
       
                  break;
                case 8: //E
                  if(nextHeading==4){
                      motion_decision = MOTION_RIGHT;
                  }else if(nextHeading == 1){
                      motion_decision = MOTION_LEFT;  
                  }else if(nextHeading == 2){
                      motion_decision = MOTION_BACK;
                  }
      
                  break;
                case 2: //W
                  if(nextHeading==1){
                      motion_decision = MOTION_RIGHT;
                  }else if(nextHeading == 4){
                      motion_decision = MOTION_LEFT;  
                  }else if(nextHeading == 8){
                      motion_decision = MOTION_BACK;
                  }
                  //turn then move forward one cell
                  break;
              }
//              nextCellflag = false;
            }
            bluetooth.print("next cell: ["); bluetooth.print(nextCoord.y);
            bluetooth.print(" "); bluetooth.print(nextCoord.x);bluetooth.println("] ");
            bluetooth.print("Current heading: "); bluetooth.println(*heading);
            curCoord = nextCoord;
            *heading = nextHeading;

            bluetooth.print("next heading: "); bluetooth.println(*heading);
            bluetooth.println();

        }
        else{
//            bluetooth.println("end of maze. ready to print.");
//            ledG::low();
//            ledR::high();
//            while(bluetooth.available()==0){}

            //GOAL FINDED, STOP CAR, end execution
            *startStep = 4;
        }
    }

    if (!motion_queue.isEmpty())
    {
    	Serial3.println("Warning! Queue not empty!");
    }
    
    motion_queue.push(&motion_decision);
	  bluetooth.print("Push: ");
	  bluetooth.println(motion_decision);
	
    if(motion_decision>=2) 
	  {
	  	motion_decision = 1;
	  	motion_queue.push(&motion_decision);
	  }
}
