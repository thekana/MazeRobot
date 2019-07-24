#include <Wire.h>
#include <avr/pgmspace.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

Maze maze;
Floodfill flood(&maze);
LinkedList<Node *> path = LinkedList<Node *>();
char commands[40]; // for storing commands
byte commandCount = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
  //  Serial3.begin(9600);
  //  Serial3.println("Ready");
}

void loop()
{
  if (Serial.available())
  {
    String c = Serial.readString();
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
    else if (c.startsWith("s"))
    {
      Serial.println(c.substring(3));
      maze.updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3));
      maze.print();
    }
    else if (c.startsWith("m"))
    {
      createPath();
      for (byte i = 0; i < path.size() - 1; i++)
      {
        byte num = path.get(i)->getValue();
        num = abs(flood.getCell(maze.getStartI(), maze.getStartJ()) - num);
        // Add to maze for printing
        maze.addPath(path.get(i)->getX(), path.get(i)->getY(), num);
      }
      // first add forward then turn
      // We need to reset commandCount before doing so
      resetCommand();
      fillCommandArray();
      maze.print();
      printCommand();
    }
    else if (c.startsWith("o")) {
      maze.markAsExplored();
    }
    else
    {
      maze.fillCells(c);
      maze.print();
    }
  }
}

void createPath()
{
  //Clear path
  clearList(&path);
  Heading h = maze.getHeading(); // starting heading
  Node *currN = nullptr;
  Node *toDelete = nullptr;
  LinkedList<Node *> stack = LinkedList<Node *>(); // Have a stack to keep all possible nodes we can traverse
  // move towards the neighbouring cells that has less than 1
  byte currI = maze.getStartI();
  byte currJ = maze.getStartJ();
  byte currV = flood.getCell(currI, currJ);
  boolean movement = true;
  while (movement)
  {
    movement = false;
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
    // At this point we should have all the cells to consider
    // Among these cells we pick the one that does not require turning (If possible)
    // And clear the stack
    if (stack.size() != 0)
    {
      byte minTurn = 100;
      byte minTurnIndex = 0;
      for (byte i = 0; i < stack.size(); i++)
      {
        byte numTurn = abs(stack.get(i)->getHead() - h);
        if (numTurn == 3) {
          numTurn = 1; // turning correction
        }
        if (minTurn > numTurn)
        {
          minTurn = numTurn;
          minTurnIndex = i;
        }
      }
      // Done looping through remove minTurnIndex
      currN = stack.remove(minTurnIndex);
      // Add to path
      path.add(currN);
      // Update values for the next iteration
      currI = currN->getX();
      currJ = currN->getY();
      currV = currN->getValue();
      h = currN->getHead();
      // Free all elems in stack
      while (stack.size() > 0)
      {
        toDelete = stack.pop();
        delete toDelete;
      }
      movement = true;
      if (currV == 0)
      {
        break;
      }
    }
  }
  clearList(&stack);
}
void clearList(LinkedList<Node *> *list)
{
  while (list->size() > 0)
  {
    Node *toDelete = list->pop();
    delete toDelete;
  }
  list->clear();
}

/*
  Assuming a path exist, commands array can be filled by calling this function
*/
void fillCommandArray()
{
  if (path.size() <= 0) return;
  Heading currHead = maze.getHeading();
  while (currHead != path.get(0)->getHead()) {
    currHead = handleTurn(currHead, path.get(0)->getHead());
  }
  for (byte i = 0; i < path.size(); i++)
  {
    addCommand(commandCount, 'F');
    if (i == path.size() - 1)
    {
      continue;
    }
    currHead = handleTurn(path.get(i)->getHead(), path.get(i + 1)->getHead());
  }
}

Heading handleTurn(Heading now, Heading next) {
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
}

void resetCommand()
{
  for (int i = 0; i < 50; i++)
  {
    commands[i] = ' ';
  }
  commandCount = 0;
}

void printCommand()
{
  Serial.println(F("Commands to destination"));
  for (int i = 0; i < commandCount; i++)
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
