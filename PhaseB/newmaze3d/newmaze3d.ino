#include <Wire.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

Maze *maze = new Maze("");
Floodfill *flood = new Floodfill(maze);
//LinkedList<Path*> pathList = LinkedList<Path*>();
LinkedList<Node*> path = LinkedList<Node*>();
void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
}

void loop()
{
  if (Serial.available()) {
    String c = Serial.readString();
    Serial.println(c);
    if (c.startsWith("f")) {
      flood->AssumeNoWalls();
      flood->doFloodfill();
      flood->print();
      flood->AssumeWalls();
      flood->doFloodfill();
      flood->print();
      Serial.print("Is maze sufficiently explored? ");
      Serial.println(flood->sufficientlyExplored());
    }
    else if (c.startsWith("p")) {
      flood->printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    }
    else if (c.startsWith("s")) {
      Serial.println(c.substring(3));
      maze->updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3));
      maze->print();
    }
    else if (c.startsWith("m")) {
      createPath();
      for (int i = 0; i < path.size(); i++) {
        path.get(i)->print();
        int num = path.get(i)->getValue();
        num = abs(flood->getCell(0, 0) - num);
        maze->addPath(path.get(i)->getX(), path.get(i)->getY(), num);
      }
      maze->print();
    }
    else {
      maze->fillCells(c);
      maze->print();
    }
  }
}

void createPath() {

  Head h = maze->getHeading();   // starting heading
  Node * currN = nullptr;
  Node * toDelete = nullptr;
  LinkedList<Node *> stack = LinkedList<Node *>();	// Have a stack to keep all possible nodes we can traverse
  // move towards the neighbouring cells that has less than 1
  int currX = maze->getStartX();
  int currY = maze->getStartY();
  int currV = flood->getCell(currX, currY);
  int movement = 1;
  while (movement != 0) {
    movement = 0;
    // for all cells neighbouring curr cells
    // add valid cells to stack
    for (int k = 0; k < 4; k++) {
      if (maze->getCellData(currX, currY, k) == 0) {
        // No wall here. Find out the cell Pose
        int tmpX = currX;
        int tmpY = currY;
        if (k == 0) {
          tmpX--;
        }
        if (k == 1) {
          tmpY++;
        }
        if (k == 2) {
          tmpX++;
        }
        if (k == 3) {
          tmpY--;
        }
        int nextV = flood->getCell(tmpX, tmpY);
        if ((currV - nextV) == 1) {
          // We want CurrV - nextCell = 1
          // this is the right cell add to stack
          stack.add(new Node(tmpX, tmpY, nextV, abs(h - k) % 2, k));
        }
      }
    }
    // At this point we should have all the cells to consider
    // Among these cells we pick the one that does not require turning
    // (If possible)
    // And clear the stack
    if (stack.size() != 0) {
      int minTurn = 100;
      int minTurnIndex = 0;
      for (int i = 0; i < stack.size(); i++) {
        if (minTurn > stack.get(i)->getTurn()) {
          minTurn = stack.get(i)->getTurn();
          minTurnIndex = i;
        }
      }
      // Done looping through remove minTurnIndex
      currN = stack.remove(minTurnIndex);
      // Add to path
      path.add(currN);
      // Update values for the next iteration
      currX = currN->getX();
      currY = currN->getY();
      currV = currN->getValue();
      h = currN->getHead();
      // Free all elems in stack
      while (stack.size() > 0) {
        toDelete = stack.pop();
        delete toDelete;
      }
      movement = 1;
      if (currV == 0) {
        break;
      }
    }
  }
  while (stack.size() > 0) {
    toDelete = stack.pop();
    delete toDelete;
  }
  stack.clear();
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
