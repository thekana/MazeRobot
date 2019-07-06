#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "Maze.h"

class Floodfill {
  private:
    Maze *maze;
    int cell[5][9];
    int rows = 5;
    int cols = 9;
    int noWallsPathCount;
    int wallsPathCount;
    bool wall = true;
  public:
    Floodfill(Maze *maze = nullptr) {
      this->maze = maze;
    }
    void AssumeNoWalls() {
      this->wall = false;
    }
    void AssumeWalls() {
      this->wall = true;
    }
    void doFloodfill() {
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          cell[i][j] = 50;
        }
      }
      cell[2][4] = 0;
      int currExploredValue = 0;
      int mazeValueChange = 1;
      while (mazeValueChange != 0) {
        mazeValueChange = 0;
        for (int i = 0; i < rows; i++) {
          for (int j = 0; j < cols; j++) {
            if (cell[i][j] == currExploredValue) {
              // Serial.print("I is: "); Serial.print(i); Serial.print(" J is: "); Serial.print(j); Serial.print(" Curr is: "); Serial.print(currExploredValue);
              // Serial.print("\n");
              for (int k = 0; k < 4 ; k++) {
                // Check unexplored wall assumption
                if (maze->getCellData(i, j, k) == -1) {
                  // unexplored wall assume no walls
                  if ( !wall ) {
                    // do update
                    mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
                  }
                } else if (maze->getCellData(i, j, k) == 0) {
                  // Serial.print("I is: "); Serial.print(i); Serial.print(" J is: "); Serial.print(j); Serial.print(" K is: "); Serial.print(k);
                  // Serial.print("\n");
                  mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
                }
              }
            }
          }
        }
        currExploredValue++;
      }
      if (wall) {
        wallsPathCount = cell[maze->getStartX()][maze->getStartY()];
      } else {
        noWallsPathCount = cell[maze->getStartX()][maze->getStartY()];
      }
    }
    int incrementNeighbour(int i, int j, int k, int value) {
      if (k == 0) {
        i = i - 1;
      } else if (k == 1) {
        j = j + 1;
      } else if (k == 2) {
        i = i + 1;
      } else {
        j = j - 1;
      }
      if (cell[i][j] == 50) {
        cell[i][j] = value + 1;
        return 1;
      } else {
        return 0;
      }
    }
    void print() {
      for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
          Serial.print(cell[i][j]);
          Serial.print("\t");
        }
        Serial.print("\n");
      }
      Serial.print("\n");
    }
    void printCell(int i, int j) {
      for (int k = 0; k < 4 ; k++) {
        Serial.print(maze->getCellData(i, j, k));
      }
      Serial.print("\n");
    }
    bool sufficientlyExplored() {
      return wallsPathCount == noWallsPathCount;
    }
    int getCell(int i, int j) {
      return cell[i][j];
    }
};
#endif
