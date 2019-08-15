// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#include "cppQueue.h" 
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
	#include "Wire.h"
#endif
#include "Sensors.h"
#include "locomotion.h"
#include "exploration.h"

// library from King's code
#include <avr/pgmspace.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

#define bluetooth Serial3
using namespace hardware;
using namespace hardware::pins;

bool map_ready=false;

float Yaw;
//Variable that indicate the car to move , stop(0), front(1), left_turning(2), right_turning(3), back_turning(4)
int motion_mode = MOTION_STOP;
static Queue motion_queue(sizeof(int));

int count=2;

int start=0;

// variables from King's code
Maze maze;
Floodfill flood(&maze);
LinkedList<Path *> path_list = LinkedList<Path *>();
char commands[50]; // for storing commands
byte commandCount = 0;
byte turnCount = 0;

int pos = 0;

// Tom set this
bool planning_done = false;

void setup() {
	// put your setup code here, to run once:
	bluetooth.begin(115200);
	//bluetooth.begin(9600);
	// bluetooth.setTimeout(100);

	bluetooth.println("Hello world!");

	// while (! bluetooth);
	bluetooth.println("Bluetooth setup finish!");

	// setup leds
	leds_setup();
	bluetooth.println("Leds setup finish!");

	// setup locomotion
	locomotion_setup();
	bluetooth.println("Locomotion setup finish!");

	//setup Sensors
	sensorSetup();
	// setup map

	// Wait 5 seconds and main loop start
	delay(1000);
}

void loop() {
	if(bluetooth.available() > 0)
	{
		String c = bluetooth.readString();
		if(c.equals("12")) start=1;
		bluetooth.println("String Received is: ");
		bluetooth.println(c);
		bluetooth.println(start);

		if (c.startsWith("b")) 
		{
			maze.fillCells(c.substring(1));
			maze.print();
		} else if (c.startsWith("c")) {
			char start_row = c.charAt(1);
			char start_col = c.charAt(2);
			String heading = String(c.charAt(3));
			char goal_row = c.charAt(4);
			char goal_col = c.charAt(5);
			maze.updateStatusCells(start_row - '0', start_col - '0', heading, goal_row - '0', goal_col - '0');
			maze.print();
			map_ready = true; 
		} else {
			// if string "12" is received
			// start 
			if (c.charAt(0) == '1' && c.charAt(1) == '2') {
				start = 1;
				bluetooth.println("Start value is: ");
				bluetooth.println(start);
			} 
		}

		char* buf = (char*) malloc(sizeof(char)*c.length()+1);

		bluetooth.println("Using toCharArray");
		c.toCharArray(buf, c.length()+1);
		bluetooth.write(buf);
		// bluetoo.println(buf);
		// Serial.println("Freeing the memory");
		free(buf);
		//Serial.println("No leaking!");
	}

	
	// put your main code here, to run repeatedly:
	double distance_f, distance_l, distance_r;
	//Array be used to detect whether there is wall on the front of sensors, left, front and right respectively, 1 have wall 0 otherwise
	int lfr[3];
	//Varianle that indicate which direction (East(1), South(2), West(3),North(4)) the car is facing
	int face_dir = 0;
	//Array be used to determine whether there is al wall at East, South, West and North
	int ESWN[4] = {0, 0, 0, 0};


	// 0. Read from bluetooth? Any command?
	if(motion_mode >= MOTION_LEFT) return;

	int current_mode = motion_mode;

	// 1. perception


	// 1.1.Read from wheel


	// 1.2.Read from sensors
	//function that dectect the left, front and right wall near the car
	//front distance (ultrasonic distance)
	distance_f = sonar<pins::sonar_trigger, pins::sonar_echo>::distance().count();        //convert the CM to MM
	//left distance (left lidar)
	distance_l = lidar<lidar_tag<0>>::distance().count();
	//right distance (right lidar)
	distance_r = lidar<lidar_tag<1>>::distance().count();
	carWall(distance_l, distance_f, distance_r, lfr);
	//update IMU parameters
	imu::update();
	//Get IMU yaw value
	Yaw = imu::yaw();
	//function that records the walls near car (Ease, South, West, North)
	ESWNWall(Yaw, lfr, ESWN, &face_dir);


	// 2. Localisation and planning

	if(map_ready && !planning_done)
	{
		planning();
		planning_done = true;
	}
	else
	{
		// exploration(motion_queue, lfr);
	}


	// 3. Give command to locomotion
	// bluetooth.print(lfr[0]);
	// bluetooth.print("******");
	// bluetooth.print(lfr[1]);
	// bluetooth.print("******");
	// bluetooth.println(lfr[2]);

	if(!start) return;
	//bluetooth.println("Motion start");
	int ncells = 0;
	// if (motion_mode == MOTION_STOP)
	// {
	// 	// Assign other values here
	// 	motion_mode = MOTION_FORWARD;
	// }

	// A simple obstacle avoidance behaviour
	// if (motion_mode == MOTION_STOP)
	// {
	//     delay(500);
	//     if(lfr[1]==0)
	//     { 
	//         bluetooth.println("Forward");
	//         motion_mode=MOTION_FORWARD;
	//     }
	//     else if(lfr[0]==0) 
	//     {
	//         bluetooth.println("Left");
	//         motion_mode=MOTION_LEFT;
	//     }
	//     else if(lfr[2]==0) 
	//     {
	//         bluetooth.println("Right");
	//         motion_mode=MOTION_RIGHT;
	//     }
	// }

	// if (motion_mode == MOTION_STOP)
	// {
	//     // Assign other values here
	//     motion_mode = MOTION_FORWARD;
	// }

	// bluetooth.println("Command count is: ");
	// bluetooth.println(commandCount);
	if (pos>=commandCount)
	{
		// bluetooth3.println("Command finish !");
		bluetooth.println("Command finish !");
		return;
	}
	
	if(motion_mode==MOTION_STOP)
	{
	  // Code from Lee
	  bluetooth.print("Command at pos is: ");
	  bluetooth.println(commands[pos]);
	  bluetooth.print("pos is: ");
	  bluetooth.println(pos);
	  if(commands[pos] == 'F')
	  {
		  // bluetooth3.println("forward");
		  bluetooth.print("forward");
		  motion_mode = MOTION_FORWARD;
		  ncells = 1;
		  pos++;
		  while(commands[pos]=='F' && pos<=commandCount)
		  {
			  ncells++;
			  pos++;
		  }
		  bluetooth.println(ncells);
	  }
	  else if (commands[pos]=='L') 
	  {
		  // bluetooth3.println("Left");
		  bluetooth.println("Left");
		  motion_mode = MOTION_LEFT;
		  pos++;
	  }
	  else if (commands[pos]=='R') 
	  {
		  // bluetooth3.println("Right");
		  bluetooth.println("Right");
		  motion_mode = MOTION_RIGHT;
		  pos++;
	  }
	}

	// motion_mode = MOTION_FORWARD;
	// delay(5000);
	// motion_mode = count;
	// bluetooth.print(count);
	// bluetooth.println("**********************");
	// count++;
	// delay(5000);
	if (motion_mode >= MOTION_LEFT)
	{
		delay(1000);
		turning();
	}
	else if (current_mode == MOTION_STOP && motion_mode == MOTION_FORWARD)
	{
		delay(1000);
		forward(ncells, distance_f, distance_l, distance_r);
	}
	else if (current_mode == MOTION_FORWARD && motion_mode == MOTION_FORWARD)
	{
		forward_updating(distance_f, distance_l, distance_r);
	}
	else
	{
		bluetooth.println("Motion status not recognized!");
	}
	// Test code
	// bluetooth.print("Front: ");
	// bluetooth.print(distance_f);
	// bluetooth.print(" Left: ");
	// bluetooth.print(distance_l);
	// bluetooth.print(" Right: ");
	// bluetooth.println(distance_r);
	// bluetooth.print(" Yaw: ");
	// bluetooth.println(Yaw);
}


void planning() 
{
  delay(1000);
  flood.AssumeWalls();
  flood.doFloodfill();
  delay(1000);
  if(!createPath()){
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
	toDelete->print();
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

// ESWN e.g. All 4 walls is F in HEX 1111
// Give Cell position 00F
//   E
//  N S
//   W
// 00B01802A03A04E11610920121A22412913C23530531B32433540341A42243244A456358343249
// --- --- --- --- --- --- --- --- ---
//| S |   *   *   *   *   *   *   *   |
// --- *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   * X *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// *** *** *** *** *** *** *** *** ***
//|   *   *   *   *   *   *   *   *   |
// --- --- --- --- --- --- --- --- ---
