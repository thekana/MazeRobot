#include <avr/pgmspace.h>
#include "Wire.h"
#include "Sensor.h"
#include "MazeExp.h"
//#include "Maze.h"
#include "LiquidCrystal_I2C.h"
#include "locomotion.h"
#include "cppQueue.h"
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"
#define bluetooth Serial3
// Define LCD pinout
const int en = 2, rw = 1, rs = 0, d4 = 4, d5 = 5, d6 = 6, d7 = 7, bl = 3;

// Define I2C Address - change if reqiuired
const int i2c_addr = 0x3F;
LiquidCrystal_I2C lcd(i2c_addr, en, rw, rs, d4, d5, d6, d7, bl, POSITIVE);

//
//hd44780_I2Cexp lcd;
//String inMSG = " ";
//const int LCD_COLS = 16;
//const int LCD_ROWS = 2;

bool nextCellflag = true;
float Yaw;
long current, timeIntervel;
String keyword;
int motion_mode = MOTION_STOP;
static Queue motion_queue(sizeof(int), 30);
//Variable that indicate the car to move , stop(0), front(1), left_turning(2), right_turning(3), back_turning(4)

extern int counterA;
extern int counterB;
int enCounter = 0;

using namespace hardware::pins;

//store the start corner of the maze
byte heading = 4;
int startStep = 0;
int cellCount = 0;
coord curCoord = {0, 0};
coord initCoord;
bool cellflag = true;
int celldis = 0;
byte mazeType = 0;
MazeExp *newMaze = new MazeExp();
bool mazeFound = false;
int lfr[3];
double distance_f, distance_l, distance_r;
int motion_decision = 0;
Maze maze;
////////////////////////////////////////////////////////////////Do not uncomment we have 2 floodfill objects now
Floodfill flood(&maze);
LinkedList<Path *> path_list = LinkedList<Path *>();
char commands[50]; // for storing commands
byte commandCount = 0;
byte turnCount = 0;
bool map_ready = false;
int start = 0;
int pos = 0;

// Tom set this
bool planning_done = false;

void setup()
{
  // put your setup code here, to run once:
  bluetooth.begin(115200);
  bluetooth.setTimeout(100);
  lcd.begin(16, 2);
  bluetooth.println();
  Serial.begin(115200);
  Wire.begin();
  Wire.setClock(400000);
  sensorSetup();
  locomotion_setup();
  //  initLCD();
  //  Serial.println("LCD   Connect!");
  //  lcd.print("Ready");
  current = millis();
  timeIntervel = 0;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HELLO");
  lcd.setCursor(0, 1);
  lcd.print("WORLD");

  bluetooth.println("Enter exploration (a), vision (b)...");
  while (bluetooth.available() == 0)
  {
    delay(100);
  } // empty buffer
  //  while (bluetooth.available()==0){delay(100);} // empty buffer
  if (bluetooth.available() > 0)
  {
    keyword = bluetooth.readString();
    if (keyword.equals("e"))
      startStep = 0;
    else if (keyword.equals("v"))
      startStep = 6;
    bluetooth.print("Heading = ");
    bluetooth.println(heading);
  }
  bluetooth.println("End setup!");

  delay(1000);
  // wait for ready
  Serial.println(F("\nSend any question number to start demo: "));
  //  while (Serial.available() && Serial.read()); // empty buffer
  //  while (!Serial.available());                 // wait for data
  ////  while (Serial.available() && Serial.read()); // empty buffer again
  //
  //  keyword = Serial.read();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void loop()
{
  distance_f = sonar<analog_pin<digital_pin<38>>, analog_pin<digital_pin<39>>>::distance().count(); //convert the CM to MM
  //left distance (left lidar)
  distance_l = lidar<lidar_tag<0>>::distance().count();
  //right distance (right lidar)
  distance_r = lidar<lidar_tag<1>>::distance().count();
  motion_decision = 0;
  int ncells;
  ncells = 0;
  int current_mode;
  current_mode = motion_mode;

  if (motion_mode >= MOTION_LEFT)
    return;
  //if can reach here, motion_mode = stop

  //function that dectect the left, front and right wall near the car
  carWall(distance_l, distance_f, distance_r, lfr);
  //Set IMU parameters
  //  imu::update();
  //  Yaw = imu::yaw();

  if (startStep == 0)
  {
    ledR::high();
    ledG::low();
    startStep = 1;
    current = millis();
    timeIntervel = 0;
  }
  else if (startStep == 1)
  {
    if (timeIntervel < 2000)
    {
      if (distance_f <= 50)
      {
        timeIntervel = millis() - current;
      }
      else
      {
        current = millis();
        timeIntervel = 0;
      }
      bluetooth.print("Sonic: ");
      bluetooth.println(distance_f);
    }
    else if (distance_f > 50)
    {
      bluetooth.println("Car start");
      ledR::low();
      ledG::high();
      delay(1000);
      startStep = 2;
    }
    else
    {
      timeIntervel = millis() - current;
    }
  }
  else if (startStep == 2)
  {
    if (lfr[2] == 0)
    {
      mazeType = 2;
      curCoord.x = 8;
      newMaze->setDestX(0);
      newMaze->setDestY(8);
      newMaze->instantiate();
      initCoord = curCoord;
      startStep = 3;
    }
    else if (lfr[0] == 1)
    {
      //        forward();

      if (motion_mode == MOTION_STOP)
      {
        if (motion_queue.isEmpty())
        {
          bluetooth.println("Queue empty!");
          motion_decision = MOTION_FORWARD;
          if (!motion_queue.isEmpty())
          {
            bluetooth.println("Warning! Queue not empty!");
          }

          motion_queue.push(&motion_decision);
          bluetooth.print("Push: ");
          bluetooth.println(motion_decision);

          if (motion_decision >= 2)
          {
            motion_decision = 1;
            motion_queue.push(&motion_decision);
          }

          cellCount++;
          bluetooth.print("CellCount: ");
          bluetooth.println(cellCount);
        }

        if (!motion_queue.isEmpty())
        {
          bluetooth.println("Delay start!");
          delay(1000);
          bluetooth.println("Delay end!");
          motion_queue.pop(&motion_mode);
          if (motion_mode == MOTION_FORWARD)
            ncells = 1;
          bluetooth.print("Pop: ");
          bluetooth.println(motion_mode);
          bluetooth.println("Queue not empty");
        }
      }
    }
    else
    {
      mazeType = 1;
      newMaze->setDestX(8);
      newMaze->setDestY(8);
      newMaze->instantiate();
      initCoord = curCoord;
      startStep = 3;
    }
  }
  else if (startStep == 3)
  {
    //    motion_decision = 0;
    if (motion_mode == MOTION_STOP)
    {
      if (motion_queue.isEmpty())
      {
        bluetooth.println("Queue empty!");
        exploration();
      }
      if (!motion_queue.isEmpty())
      {
        bluetooth.println("Delay start!");
        delay(1000);
        bluetooth.println("Delay end!");
        motion_queue.pop(&motion_mode);
        if (motion_mode == MOTION_FORWARD)
          ncells = 1;

        bluetooth.print("Pop: ");
        bluetooth.println(motion_mode);
        bluetooth.println("Queue not empty");
      }
    }
  }
  else if (startStep == 4)
  {
    maze.updateStatusCells(initCoord.y, initCoord.x, "W", newMaze->getDestY(), newMaze->getDestX());
    maze.print();
    bluetooth.println("End");
    planning();
    startStep = 5;
    delay(5000);
  }
  else if (startStep == 5)
  {
    ledR::low();
    ledG::high();
    //bluetooth.println("In step 5");
    if (motion_mode == MOTION_STOP)
    {
      if (!motion_queue.isEmpty())
      {
        delay(500);
        motion_queue.pop(&motion_mode);
        if (motion_mode > 10)
        {
          ncells = motion_mode - 10;
          motion_mode = MOTION_FORWARD;
        }
        bluetooth.println(motion_mode);
      }
      if (motion_queue.isEmpty())
      {
        startStep = 6;
        ledR::high();
        ledG::low();
      }
    }
  }
  else if (startStep == 6)
  {
    if (bluetooth.available() > 0)
    {
      String c = bluetooth.readString();
      if (c.equals("12"))
        start = 1;
      bluetooth.println("String Received is: ");
      bluetooth.println(c);
      bluetooth.println(start);

      if (c.startsWith("b"))
      {
        maze.fillCells(c.substring(1));
        maze.print();
      }
      else if (c.startsWith("c"))
      {
        char start_row = c.charAt(1);
        char start_col = c.charAt(2);
        String heading = String(c.charAt(3));
        char goal_row = c.charAt(4);
        char goal_col = c.charAt(5);
        maze.updateStatusCells(start_row - '0', start_col - '0', heading, goal_row - '0', goal_col - '0');
        maze.print();
        map_ready = true;
      }
      else
      {
        // if string "12" is received
        // start
        if (c.charAt(0) == '1' && c.charAt(1) == '2')
        {
          start = 1;
          bluetooth.println("Start value is: ");
          bluetooth.println(start);
        }
      }

      char *buf = (char *)malloc(sizeof(char) * c.length() + 1);

      bluetooth.println("Using toCharArray");
      c.toCharArray(buf, c.length() + 1);
      bluetooth.write(buf);
      // bluetoo.println(buf);
      // Serial.println("Freeing the memory");
      free(buf);
      //Serial.println("No leaking!");
    }

    if (!start)
      return;
    //bluetooth.println("Motion start");
    int ncells = 0;
    if (pos >= commandCount)
    {
      // bluetooth3.println("Command finish !");
      bluetooth.println("Command finish !");
      return;
    }

    if (motion_mode == MOTION_STOP)
    {
      // Code from Lee
      bluetooth.print("Command at pos is: ");
      bluetooth.println(commands[pos]);
      bluetooth.print("pos is: ");
      bluetooth.println(pos);
      if (commands[pos] == 'F')
      {
        // bluetooth3.println("forward");
        bluetooth.print("forward");
        motion_mode = MOTION_FORWARD;
        ncells = 1;
        pos++;
        while (commands[pos] == 'F' && pos <= commandCount)
        {
          ncells++;
          pos++;
        }
        bluetooth.println(ncells);
      }
      else if (commands[pos] == 'L')
      {
        // bluetooth3.println("Left");
        bluetooth.println("Left");
        motion_mode = MOTION_LEFT;
        pos++;
      }
      else if (commands[pos] == 'R')
      {
        // bluetooth3.println("Right");
        bluetooth.println("Right");
        motion_mode = MOTION_RIGHT;
        pos++;
      }
    }
  }

  if (motion_mode >= MOTION_LEFT)
  {
    //         bluetooth.println("Turning");
    turning();
  }
  else if (current_mode == MOTION_STOP && motion_mode == MOTION_FORWARD)
  {
    //        bluetooth.println("Forward");
    forward(ncells, distance_f, distance_l, distance_r);
  }
  else if (current_mode == MOTION_FORWARD && motion_mode == MOTION_FORWARD)
  {
    forward_updating(distance_f, distance_l, distance_r);
  }
  else
  {
    Serial.println("Motion status not recognized!");
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void exploration()
{
  //  bluetooth.print("MazeType: ");bluetooth.println(mazeType);
  if (cellCount != 0)
  {
    curCoord.y = cellCount;
    for (int i = 0; i < cellCount; i++)
    {
      newMaze->maze[i][curCoord.x].walls = 10;
      newMaze->maze[i][curCoord.x].detected = true;
      maze.fillCells_steven(i, curCoord.x, newMaze->maze[i][curCoord.x].walls);
    }
    cellCount = 0;
  }
  if (newMaze->maze[curCoord.y][curCoord.x].distance != 0)
  {
    //      bluetooth.print("Current cell distance: ");  bluetooth.println(newMaze->maze[curCoord.y][curCoord.x].distance);
    bluetooth.print("front: ");
    bluetooth.println(distance_f);
    bluetooth.print("left: ");
    bluetooth.println(distance_l);
    bluetooth.print("right: ");
    bluetooth.println(distance_r);
    bluetooth.print("Wall: ");
    bluetooth.print(lfr[0]);
    bluetooth.print(lfr[1]);
    bluetooth.println(lfr[2]);
    bluetooth.print("Current cell: [");
    bluetooth.print(curCoord.y);
    bluetooth.print(" ");
    bluetooth.print(curCoord.x);
    bluetooth.println("] ");
    newMaze->floodFillUpdate(curCoord, heading, lfr);
    newMaze->maze[curCoord.y][curCoord.x].detected = true;
    maze.fillCells_steven(curCoord.y, curCoord.x, newMaze->maze[curCoord.y][curCoord.x].walls);
    maze.print();
    int nextHeading = newMaze->orient(curCoord, heading);
    coord nextCoord = newMaze->bearingCoord(curCoord, nextHeading);

    //      bluetooth.print("next cell: ["); bluetooth.print(nextCoord.y);
    //      bluetooth.print(" "); bluetooth.print(nextCoord.x);bluetooth.println("] ");
    bluetooth.print("Current heading: ");
    bluetooth.println(heading);
    curCoord = nextCoord;

    if (heading == nextHeading)
    {
      //Move forward one cell
      motion_decision = MOTION_FORWARD;
    }
    else
    {
      switch (heading)
      {
      case 1: //N
        if (nextHeading == 2)
        {
          motion_decision = MOTION_LEFT;
        }
        else if (nextHeading == 4)
        {
          motion_decision = MOTION_BACK;
        }
        else if (nextHeading == 8)
        {
          motion_decision = MOTION_RIGHT;
        }
        break;
      case 4: //S
        if (nextHeading == 2)
        {
          motion_decision = MOTION_RIGHT;
        }
        else if (nextHeading == 8)
        {
          motion_decision = MOTION_LEFT;
        }
        else if (nextHeading == 1)
        {
          motion_decision = MOTION_BACK;
        }

        break;
      case 8: //E
        if (nextHeading == 4)
        {
          motion_decision = MOTION_RIGHT;
        }
        else if (nextHeading == 1)
        {
          motion_decision = MOTION_LEFT;
        }
        else if (nextHeading == 2)
        {
          motion_decision = MOTION_BACK;
        }

        break;
      case 2: //W
        if (nextHeading == 1)
        {
          motion_decision = MOTION_RIGHT;
        }
        else if (nextHeading == 4)
        {
          motion_decision = MOTION_LEFT;
        }
        else if (nextHeading == 8)
        {
          motion_decision = MOTION_BACK;
        }
        //turn then move forward one cell
        break;
      }
      //              nextCellflag = false;
    }
    heading = nextHeading;
    bluetooth.print("next heading: ");
    bluetooth.println(heading);
    bluetooth.println();
    if (mazeType == 1 && mazeFound == false)
    {
      bluetooth.println("calc distance!");
      if (curCoord.x > 4)
      {
        newMaze->setDestX(4);
        newMaze->setDestY(2);
        newMaze->reloadDist();
        newMaze->floodFillUpdate(curCoord, heading, lfr);
        mazeFound = true;
        bluetooth.print("DestX: ");
        bluetooth.println(newMaze->getDestX());
        bluetooth.print("DestY: ");
        bluetooth.println(newMaze->getDestY());
      }
      else if (curCoord.y > 4)
      {
        newMaze->setDestX(2);
        newMaze->setDestY(4);
        newMaze->reloadDist();
        newMaze->floodFillUpdate(curCoord, heading, lfr);
        mazeFound = true;
        bluetooth.print("DestX: ");
        bluetooth.println(newMaze->getDestX());
        bluetooth.print("DestY: ");
        bluetooth.println(newMaze->getDestY());
      }
    }
    else if (mazeType == 2 && mazeFound == false)
    {
      bluetooth.println("calc distance!");
      if (curCoord.x < 4)
      {
        newMaze->setDestX(4);
        newMaze->setDestY(2);
        newMaze->reloadDist();
        mazeFound = true;
        bluetooth.print("DestX: ");
        bluetooth.println(newMaze->getDestX());
        bluetooth.print("DestY: ");
        bluetooth.println(newMaze->getDestY());
      }
      else if (curCoord.y > 4)
      {
        newMaze->setDestX(6);
        newMaze->setDestY(4);
        newMaze->reloadDist();
        mazeFound = true;
        bluetooth.print("DestX: ");
        bluetooth.println(newMaze->getDestX());
        bluetooth.print("DestY: ");
        bluetooth.println(newMaze->getDestY());
      }
    }
    //      delay(6000);
  }
  else
  {
    bluetooth.println("end of maze. ready to print.");
    motion_mode = MOTION_STOP;
    newMaze->floodFillUpdate(curCoord, heading, lfr);
    maze.fillCells_steven(curCoord.y, curCoord.x, newMaze->maze[curCoord.y][curCoord.x].walls);
    maze.print();
    ledG::low();
    ledR::high();
    while (bluetooth.available() == 0)
    {
    }

    //GOAL FINDED, STOP CAR, end execution
    startStep = 4;
  }

  if (!motion_queue.isEmpty())
  {
    bluetooth.println("Warning! Queue not empty!");
    delay(100000);
  }

  motion_queue.push(&motion_decision);
  bluetooth.print("Push: ");
  bluetooth.println(motion_decision);

  if (motion_decision >= 2)
  {
    motion_decision = 1;
    motion_queue.push(&motion_decision);
  }
}

void planning()
{
  delay(1000);
  flood.AssumeWalls();
  flood.doFloodfill();
  delay(1000);
  if (!createPath())
  {
    return;
  }
  bluetooth.print("Total Path: ");
  bluetooth.println(path_list.size());
  assignCostToEachPath();
  bluetooth.println("Assigned Path");
  //Now we pick the path with least turns
  Path *bestPath = path_list.get(0);
  for (byte i = 0; i < path_list.size(); i++)
  {
    if (bestPath->actionCount > path_list.get(i)->actionCount)
    {
      bestPath = path_list.get(i);
    }
  }
  for (byte i = 0; i < bestPath->nodeList->size() - 1; i++)
  {
    byte num = bestPath->nodeList->get(i)->getValue();
    num = abs(flood.getCell(maze.getStartI(), maze.getStartJ()) - num);
    // Add to maze for printing
    maze.addPath(bestPath->nodeList->get(i)->getX(), bestPath->nodeList->get(i)->getY(), num);
  }
  // first add forward then turn
  // We need to reset commandCount before doing so
  resetCommand();
  fillCommandArray(bestPath->nodeList);
  maze.print();
  printCommand();
  // Once we have got the command array need to free up everything
  bluetooth.println("Going to clear Path List");
  clearPathList();
  bluetooth.println("Cleared Path List");
  fillMotionQueue();
}

// functions from King
Path *getCloneOfPath(Path *other)
{
  Path *clone = new Path();
  for (byte i = 0; i < other->nodeList->size() - 1; i++)
  {
    clone->add(other->nodeList->get(i));
  }
  return clone;
}
//Return true if every path in path_list is completed
bool everyPathComplete()
{
  for (byte i = 0; i < path_list.size(); i++)
  {
    if (path_list.get(i)->completed == 0)
    {
      return false;
    }
  }
  return true;
}
byte createPath()
{

  if (flood.getCell(maze.getStartI(), maze.getStartJ()) == MAX_CELL_VALUE)
  {
    Serial.println(F("There is no path"));
    return 0; //fail
  }

  LinkedList<Node *> stack = LinkedList<Node *>(); // Have a stack to keep all possible nodes we can traverse
  // move towards the neighbouring cells that has less than 1
  byte currI = maze.getStartI();
  byte currJ = maze.getStartJ();
  byte currV = flood.getCell(currI, currJ);
  Heading h = maze.getHeading(); // starting heading
  Path *firstPath = new Path();
  firstPath->add(new Node(currI, currJ, currV, h));
  path_list.add(firstPath);
  while (!everyPathComplete())
  {
    //Explore every possible path
    for (byte i = 0; i < path_list.size(); i++)
    {
      Path *currPath = path_list.get(i);
      currI = currPath->getLastX();
      currJ = currPath->getLastY();
      currV = currPath->getLastValue();
      h = currPath->getLastHeading();
      if (currV == 0)
      {
        //We have fully explored this path
        currPath->completed = 1;
        continue;
      }
      // for all cells neighbouring curr cells
      // add valid cells to stack
      for (byte k = 0; k < 4; k++)
      {
        if (maze.hasWall(currI, currJ, k) == 0)
        {
          // No wall here. Find out the cell Pose
          byte tmpI = currI;
          byte tmpJ = currJ;
          switch (k)
          {
          case NORTH:
            tmpJ--;
            break;
          case SOUTH:
            tmpJ++;
            break;
          case EAST:
            tmpI--;
            break;
          case WEST:
            tmpI++;
            break;
          default:
            break;
          }
          byte nextV = flood.getCell(tmpI, tmpJ);
          if ((currV - nextV) == 1)
          {
            // We want CurrV - nextCell = 1
            // this is the right cell add to stack
            stack.add(new Node(tmpI, tmpJ, nextV, k));
          }
        }
      }
      // Add to path
      currPath->add(stack.pop());
      // In a case where the stack size isnt zero
      // new path needs to be created and added to path list
      while (stack.size() > 0)
      {
        Path *new_path = getCloneOfPath(currPath);
        new_path->add(stack.pop());
        path_list.add(new_path);
      }
    }
  }
  stack.clear();
  return 1;
}
void clearPathList()
{
  //Create a list that points to all other nodes
  LinkedList<Node *> temp = LinkedList<Node *>();
  for (byte i = 0; i < path_list.size(); i++)
  {
    Path *curr = path_list.get(i);
    for (byte j = 0; j < curr->nodeList->size(); j++)
    {
      Node *currNode = curr->nodeList->get(j);
      byte isInTemp = 0;
      for (byte k = 0; k < temp.size(); k++)
      {
        if (temp.get(k)->getX() == currNode->getX() && temp.get(k)->getY() == currNode->getY())
        {
          isInTemp = 1;
          break;
        }
      }
      if (!isInTemp)
      {
        temp.add(currNode);
      }
    }
  }
  while (path_list.size() > 0)
  {
    Path *toDelete = path_list.pop();
    toDelete->clearNodeList();
    delete toDelete;
  }
  while (temp.size() > 0)
  {
    Node *toDelete = temp.pop();
    //toDelete->print();
    delete toDelete;
  }
  path_list.clear();
}
void assignCostToEachPath()
{
  for (byte i = 0; i < path_list.size(); i++)
  {
    resetCommand();
    Path *currPath = path_list.get(i);
    fillCommandArray(currPath->nodeList);
    currPath->actionCount = turnCount;
  }
}
/*
  Assuming a path exist, commands array can be filled by calling this function
*/
void fillCommandArray(LinkedList<Node *> *path)
{
  // Ones to exclude starting cell
  if (path->size() <= 1)
    return;
  Heading currHead = maze.getHeading();
  while (currHead != path->get(1)->getHead())
  {
    currHead = handleTurn(currHead, path->get(1)->getHead());
  }
  for (byte i = 1; i < path->size(); i++)
  {
    addCommand(commandCount, 'F');
    if (i == path->size() - 1)
    {
      continue;
    }
    currHead = handleTurn(path->get(i)->getHead(), path->get(i + 1)->getHead());
  }
}

Heading handleTurn(Heading now, Heading next)
{
  if (now == NORTH && next == EAST)
  {
    addCommand(commandCount, 'R');
    return EAST;
  }
  else if (now == NORTH && next == WEST)
  {
    addCommand(commandCount, 'L');
    return WEST;
  }
  else if (now == EAST && next == NORTH)
  {
    addCommand(commandCount, 'L');
    return NORTH;
  }
  else if (now == EAST && next == SOUTH)
  {
    addCommand(commandCount, 'R');
    return SOUTH;
  }
  else if (now > next)
  {
    addCommand(commandCount, 'R');
    return now - 1;
  }
  else if (now < next)
  {
    addCommand(commandCount, 'L');
    return now + 1;
  }
}

void addCommand(byte i, char c)
{
  commands[i] = c;
  commandCount++;
  if (c != 'F')
  {
    turnCount++;
  }
}

void resetCommand()
{
  for (byte i = 0; i < 50; i++)
  {
    commands[i] = ' ';
  }
  commandCount = 0;
  turnCount = 0;
}

void printCommand()
{
  bluetooth.println(F("Commands to destination"));
  for (int i = 0; i < commandCount; i++)
  {
    bluetooth.print(commands[i]);
    bluetooth.print("  ");
  }
  bluetooth.println("");
}

void fillMotionQueue()
{
  if (commandCount == 0)
  {
    return; //error
  }
  byte forwardCount = 0;
  motion_decision = 0;
  for (byte i = 0; i < commandCount; i++)
  {
    switch (commands[i])
    {
    case 'F':
      forwardCount++;
      break;
    case 'R':
      if (forwardCount != 0)
      {
        motion_decision = 10 + forwardCount;
        motion_queue.push(&motion_decision);
        forwardCount = 0;
      }
      motion_decision = 3;
      motion_queue.push(&motion_decision);
      break;
    case 'L':
      if (forwardCount != 0)
      {
        motion_decision = 10 + forwardCount;
        motion_queue.push(&motion_decision);
        forwardCount = 0;
      }
      motion_decision = 2;
      motion_queue.push(&motion_decision);
      break;
    }
  }
  if (forwardCount != 0)
  {
    motion_decision = 10 + forwardCount;
    motion_queue.push(&motion_decision);
    forwardCount = 0;
  }
  // int motion_mode;
  // while (!motion_queue.isEmpty())
  // {
  //   motion_queue.pop(&motion_mode);
  //   Serial.print(motion_mode);
  //   Serial.print(" ");
  // }
  // Serial.println("End of Queue");
}

void forward()
{
  counterA = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Move");
  lcd.setCursor(0, 1);
  lcd.print("Forward");
  while (counterA < 38)
  {
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    delay(500);
  }
  //  bluetooth.println("Forward");
}

void turnR()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Right");
  counterA = 0;
  counterB = 0;
  while (true)
  {
    if ((counterA >= 12) || (counterB <= -12))
    {
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}

void turnL()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Left");
  counterA = 0;
  counterB = 0;
  while (true)
  {
    if ((counterA <= -12) || (counterB >= 12))
    {
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}
void turnB()
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turn");
  lcd.setCursor(0, 1);
  lcd.print("Back");
  counterA = 0;
  counterB = 0;
  while (true)
  {
    if ((counterA <= -24) || (counterB >= 24))
    {
      break;
    }
    bluetooth.print("CounterA: ");
    bluetooth.println(counterA);
    bluetooth.print("CounterB: ");
    bluetooth.println(counterB);
    delay(500);
  }
}
