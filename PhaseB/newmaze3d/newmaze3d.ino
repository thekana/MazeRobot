#include <Wire.h>
#include "Maze.h"
#include "Floodfill.h"
#include "Path.h"

Maze *lay = new Maze("");
Floodfill *ff = new Floodfill(lay);
LinkedList<Path*> pathList = LinkedList<Path*>();

void setup()
{
  Serial.begin(9600);
  Serial.println("Hello");
  pathList.add(new Path()); // added 1 path to the list
}

void loop()
{
  if (Serial.available()) {
    String c = Serial.readString();
    Serial.println(c);
    if (c.startsWith("f")) {
      ff->AssumeNoWalls();
      ff->doFloodfill();
      ff->print();
      ff->AssumeWalls();
      ff->doFloodfill();
      ff->print();
      Serial.print("Is maze sufficiently explored? ");
      Serial.println(ff->sufficientlyExplored());
    }
    else if (c.startsWith("P")) {
      ff->printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    }
    else if (c.startsWith("S")) {
      Serial.println(c.substring(3));
      lay->updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3));
      lay->print();
    }
    else if (c.startsWith("M")) {
      createPath();
      pathList.get(0)->print();
    }
    else {
      lay->fillCells(c);
      lay->print();
    }
    //maze.setMessage(c);
    //maze_layout layout(maze.rows, maze.cols, maze.hWall, maze.vWall);
    //layout.print();
  }
}

void newPath() {
  pathList.add(new Path());
}

void createPath() {
  Path * currP = pathList.get(0);
  Head h = lay->getHeading();   // starting heading
  Node * currN = nullptr;
  Node * toDelete = nullptr;
  // move towards the neighbouring cells that has less than 1
  int currX = lay->getStartX();
  int currY = lay->getStartY();
  int currV = ff->getCell(currX, currY);
  int movement = 1;
  // Have a stack to keep all possible nodes we can traverse
  LinkedList<Node *> stack = LinkedList<Node *>();
  while (movement != 0) {
    movement = 0;
    // for all cells neighbouring curr cells
    // add valid cells to stack
    for (int k = 0; k < 4; k++) {
      if (lay->getCellData(currX, currY, k) == 0) {
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
        int nextV = ff->getCell(tmpX, tmpY);
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
	  Serial.println(stack.size());
      for (int i = 0; i < stack.size(); i++) {
        if (minTurn > stack.get(i)->getTurn()) {
          minTurn = stack.get(i)->getTurn();
          minTurnIndex = i;
        }
      }
      // Done looping through remove minTurnIndex
      currN = stack.remove(minTurnIndex);
      // Add to path
      currP->add(currN);
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
}
// ESWN e.g. All 4 walls is F in HEX 1111
// Give Cell position 00F
//   E
//  N S
//   W
// 00B01002A03A04E116109
// 00B01002A03A04E11610920021A22412913C235
// 00B01002A03A04E11610920021A22412913C23530531B32433540341A42243244A
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
