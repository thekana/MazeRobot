#include <Wire.h>
#include <avr/pgmspace.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

Maze maze;
Floodfill flood(&maze);
LinkedList<Path *> path_list = LinkedList<Path *>();

void setup()
{
  Serial.begin(115200);
  Serial.println("Hello");
  Serial.println(sizeof(path_list));
  Serial.println(sizeof(maze));
  Serial.println(sizeof(flood));
  //  Serial3.begin(115200);
  //  Serial3.println("Ready");
}
String c;
void loop()
{
  if (Serial.available())
  {
    c = Serial.readString();
    Serial.println(c);
    if (c.startsWith("f"))
    {
      flood.AssumeNoWalls();
      flood.doFloodfill();
      flood.print();
      flood.AssumeWalls();
      flood.doFloodfill();
      flood.print();
      Serial.print(F("Is maze sufficiently explored? "));
      Serial.println(flood.sufficientlyExplored());
    }
    else if (c.startsWith("p"))
    {
      flood.printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    }
    else if (c.startsWith("t"))
    {
      Serial.println(c.substring(3));
      maze.updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3, 4), c.charAt(4) - '0', c.charAt(5) - '0');
      maze.print();
    }
    else if (c.startsWith("e"))
    {
      Serial.println(c.substring(3));
      maze.updateEast(c.charAt(1) - '0', c.charAt(2) - '0', c.charAt(3) - '0');
      maze.print();
    }
    else if (c.startsWith("s"))
    {
      Serial.println(c.substring(3));
      maze.updateSouth(c.charAt(1) - '0', c.charAt(2) - '0', c.charAt(3) - '0');
      maze.print();
    }
    else if (c.startsWith("w"))
    {
      Serial.println(c.substring(3));
      maze.updateWest(c.charAt(1) - '0', c.charAt(2) - '0', c.charAt(3) - '0');
      maze.print();
    }
    else if (c.startsWith("n"))
    {
      Serial.println(c.substring(3));
      maze.updateNorth(c.charAt(1) - '0', c.charAt(2) - '0', c.charAt(3) - '0');
      maze.print();
    }
    else if (c.startsWith("m"))
    {
      char commands[40];
      byte commandCount = 0;
      byte turnCount = 0;
      createPath();
      // Serial.print("Total Path: ");
      // Serial.println(path_list.size());
      assignCostToEachPath(commands, commandCount, turnCount);
      // Serial.println("Assigned Path");
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
      resetCommand(commands, commandCount, turnCount);
      fillCommandArray(bestPath->nodeList, commands, commandCount, turnCount);
      maze.print();
      printCommand(commands, commandCount);
      // Once we have got the command array need to free up everything
      Serial.println("Going to clear Path List");
      clearPathList();
      Serial.println("Cleared Path List");
    }
    else if (c.startsWith("o"))
    {
      maze.markAsExplored();
    }
    else
    {
      maze.fillCells(c);
      maze.print();
    }
  }
}

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
void createPath()
{
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
void assignCostToEachPath(char *commands, byte *commandCount, byte *turnCount)
{
  for (byte i = 0; i < path_list.size(); i++)
  {
    resetCommand(commands, commandCount, turnCount);
    Path *currPath = path_list.get(i);
    fillCommandArray(currPath->nodeList, commands, commandCount, turnCount);
    currPath->actionCount = *turnCount;
  }
}
/*
  Assuming a path exist, commands array can be filled by calling this function
*/
void fillCommandArray(LinkedList<Node *> *path, char *commands, byte *commandCount, byte *turnCount)
{
  // Ones to exclude starting cell
  if (path->size() <= 1)
    return;
  Heading currHead = maze.getHeading();
  while (currHead != path->get(1)->getHead())
  {
    currHead = handleTurn(currHead, path->get(1)->getHead(), commands, commandCount, turnCount);
  }
  for (byte i = 1; i < path->size(); i++)
  {
    addCommand('F', commands, commandCount, turnCount);
    if (i == path->size() - 1)
    {
      continue;
    }
    currHead = handleTurn(path->get(i)->getHead(), path->get(i + 1)->getHead(), commands, commandCount, turnCount);
  }
}

Heading handleTurn(Heading now, Heading next, char *commands, byte *commandCount, byte *turnCount)
{
  if (now == NORTH && next == EAST)
  {
    addCommand('R', commands, commandCount, turnCount);
    return EAST;
  }
  else if (now == NORTH && next == WEST)
  {
    addCommand('L', commands, commandCount, turnCount);
    return WEST;
  }
  else if (now == EAST && next == NORTH)
  {
    addCommand('L', commands, commandCount, turnCount);
    return NORTH;
  }
  else if (now == EAST && next == SOUTH)
  {
    addCommand('R', commands, commandCount, turnCount);
    return SOUTH;
  }
  else if (now > next)
  {
    addCommand('R', commands, commandCount, turnCount);
    return now - 1;
  }
  else if (now < next)
  {
    addCommand('L', commands, commandCount, turnCount);
    return now + 1;
  }
}

void addCommand(char c, char *commands, byte *commandCount, byte *turnCount)
{
  commands[*commandCount] = c;
  (*commandCount)++;
  if (c != 'F')
  {
    (*turnCount)++;
  }
}

void resetCommand(char *commands, byte *commandCount, byte *turnCount)
{
  for (byte i = 0; i < 50; i++)
  {
    commands[i] = ' ';
  }
  *commandCount = 0;
  *turnCount = 0;
}

void printCommand(char *commands, byte *commandCount)
{
  Serial.println(F("Commands to destination"));
  for (int i = 0; i < *commandCount; i++)
  {
    Serial.print(commands[i]);
    Serial.print("  ");
  }
  Serial.println("");
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
