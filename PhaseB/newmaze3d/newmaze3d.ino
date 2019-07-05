#include <Wire.h>
#include "Maze.h"
#include "Floodfill.h"


Maze *lay = new Maze("");
Floodfill ff(lay);
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
      ff.AssumeNoWalls();
      ff.doFloodfill();
      ff.print();
      ff.AssumeWalls();
      ff.doFloodfill();
      ff.print();
      Serial.print("Is maze sufficiently explored? ");
      Serial.println(ff.sufficientlyExplored());
    } else if (c.startsWith("P")) {
      ff.printCell(c.charAt(1) - '0', c.charAt(2) - '0');
    } else if (c.startsWith("S")) {
      Serial.println(c.substring(3));
      lay->updateStatusCells(c.charAt(1) - '0', c.charAt(2) - '0', c.substring(3));
      lay->print();
    } else {
      lay->fillCells(c);
      lay->print();
    }
    //maze.setMessage(c);
    //maze_layout layout(maze.rows, maze.cols, maze.hWall, maze.vWall);
    //layout.print();
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
// 00B01802A03A04E11610920021A22412913C23530531B32433540341A42243244A456358343249
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
