#include <Wire.h>
#include <avr/pgmspace.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

Maze *maze = new Maze();
Floodfill *flood = new Floodfill(maze);
LinkedList<Node *> path = LinkedList<Node *>();
//LinkedList<char> *commands = new LinkedList<char>();
char commands[40]; // for storing commands
byte commandCount = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
}

void loop()
{
  if (Serial.available())
  {
    String c = Serial.readString();
    Serial.println(c);
    if (c.startsWith("f"))
    {
      flood->AssumeNoWalls();
      flood->doFloodfill();
      flood->print();
      flood->AssumeWalls();
      flood->doFloodfill();
      flood->print();
      Serial.print(F("Is maze sufficiently explored? "));
      Serial.println(flood->sufficientlyExplored());
    }
    else if (c.startsWith("p"))
    {
      flood->printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    }
    else if (c.startsWith("s"))
    {
      Serial.println(c.substring(3));
      maze->updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3));
      maze->print();
    }
    else if (c.startsWith("m"))
    {
      createPath();
      for (byte i = 0; i < path.size(); i++)
      {
        //path.get(i)->print();
        byte num = path.get(i)->getValue();
        num = abs(flood->getCell(maze->getStartI(), maze->getStartJ()) - num);
        // Add to maze for printing
        maze->addPath(path.get(i)->getX(), path.get(i)->getY(), num);
      }
      // first add forward then turn
      // We need to reset commandCount before doing so
      resetCommand();
      fillCommandArray();
      maze->print();
      printCommand();
    }
    else
    {
      maze->fillCells(F("00B01802A03A04E11610920121A22412913C23530531B32433540341A42243244A456358343249"));
      maze->print();
    }
  }
}

void createPath()
{
  //Clear path
  clearList(&path);
  Heading h = maze->getHeading(); // starting heading
  Node *currN = nullptr;
  Node *toDelete = nullptr;
  LinkedList<Node *> stack = LinkedList<Node *>(); // Have a stack to keep all possible nodes we can traverse
  // move towards the neighbouring cells that has less than 1
  byte currI = maze->getStartI();
  byte currJ = maze->getStartJ();
  byte currV = flood->getCell(currI, currJ);
  boolean movement = true;
  while (movement)
  {
    movement = false;
    // for all cells neighbouring curr cells
    // add valid cells to stack
    for (byte k = 0; k < 4; k++)
    {
      if (maze->hasWall(currI, currJ, k) == 0)
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
        byte nextV = flood->getCell(tmpI, tmpJ);
        if ((currV - nextV) == 1)
        {
          // We want CurrV - nextCell = 1
          // this is the right cell add to stack
          stack.add(new Node(tmpI, tmpJ, nextV, abs(h - k) % 2, k));
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
        if (minTurn > stack.get(i)->getTurn())
        {
          minTurn = stack.get(i)->getTurn();
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
  for (byte i = 0; i < path.size(); i++)
  {
    addCommand(commandCount, 'F');
    if (i == path.size() - 1)
    {
      continue;
    }
    if (path.get(i)->getHead() == NORTH && path.get(i + 1)->getHead() == EAST)
    {
      addCommand(commandCount, 'R');
    }
    else if (path.get(i)->getHead() == NORTH && path.get(i + 1)->getHead() == WEST)
    {
      addCommand(commandCount, 'L');
    }
    else if (path.get(i)->getHead() == EAST && path.get(i + 1)->getHead() == NORTH)
    {
      addCommand(commandCount, 'L');
    }
    else if (path.get(i)->getHead() == EAST && path.get(i + 1)->getHead() == SOUTH)
    {
      addCommand(commandCount, 'R');
    }
    else if (path.get(i)->getHead() > path.get(i + 1)->getHead())
    {
      addCommand(commandCount, 'R');
    }
    else if (path.get(i)->getHead() < path.get(i + 1)->getHead())
    {
      addCommand(commandCount, 'L');
    }
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
