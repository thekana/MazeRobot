#ifndef MAZE_H
#define MAZE_H
#include <Arduino.h>

int x2i(char *s)
{
  int x = 0;
  for (;;) {
    char c = *s;
    if (c >= '0' && c <= '9') {
      x *= 16;
      x += c - '0';
    }
    else if (c >= 'A' && c <= 'F') {
      x *= 16;
      x += (c - 'A') + 10;
    }
    else break;
    s++;
  }
  return x;
}
enum heading {
  EAST = 0,
  SOUTH = 1,
  WEST = 2,
  NORTH = 3
};
typedef enum heading Head;

class Maze {
  private:
    int cells[5][9][4];
    String statusCells[5][9];
    int rows = 5;
    int cols = 9;
    int startX = 0;
    int startY = 0;
    Head head;
  public:
    Maze(String const &h) {
      this->rows = 5;
      this->cols = 9;
      initializeCells();  // -1 is unexplored
      initializeStatusCells(); // 4 spaces
      statusCells[2][4] = " X ";  // mark goal
    }
    void initializeCells() {
      for (int i = 0; i < 5 ; i++) {
        for (int j = 0; j < 9; j++) {
          for (int k = 0; k < 4; k++) {
            cells[i][j][k] = -1;
            if (i == 0) {
              cells[i][j][0] = 1;
            }
            if (i == 4) {
              cells[i][j][2] = 1;
            }
            if (j == 0) {
              cells[i][j][3] = 1;
            }
            if (j == 8) {
              cells[i][j][1] = 1;
            }
          }
        }
      }
    }
    void initializeStatusCells() {
      for (int i = 0; i < 5 ; i++) {
        for (int j = 0; j < 9; j++) {
          // set every cells to 4 spaces
          statusCells[i][j] = "   ";
        }
      }
    }
    void updateStatusCells(int i, int j, String const&h) {
      // to update these cells with path
      // data and start/end
      if (h == "E") {
        head = EAST;
      } else if (h == "S") {
        head = SOUTH;
      } else if (h == "W") {
        head = WEST;
      } else if (h == "N") {
        head = NORTH;
      }
      statusCells[i][j] = String(" " + h + " ");
      //      Serial.print("Updated");
      //      Serial.print(i); Serial.print(j); Serial.println(statusCells[i][j]);
    }
    void addPath(int i, int j, int val) {
      Serial.println(val);
      String space = " ";
      String str = String(val);
      space.concat(str);
      if (val < 10) {
        space.concat(" ");
      }
      statusCells[i][j] = space;
    }
    void fillCells(String const& h) {
      int index = 0;
      int len = h.length();
      while (index < len) {
        int i = h.charAt(index) - '0';
        index++;
        int j = h.charAt(index) - '0';
        index++;
        char a = h.charAt(index);
        int data = x2i(&a);
        index++;
        // Now update the cell data
        // int data = decimal * 10 + digit;
        // BITMASK E-S-W-N
        cells[i][j][0] = (data & B1000) >> 3;
        cells[i][j][1] = (data & B0100) >> 2;
        cells[i][j][2] = (data & B0010) >> 1;
        cells[i][j][3] = (data & B0001) >> 0;
        updateAdjCells(i, j, data);
      }
    }
    void updateAdjCells(int i, int j, int data) {
      if ( i >= 0 && i < 4 ) {
        // can go down
        cells[i + 1][j][0] = cells[i][j][2];
      }
      if ( i <= 4 && i > 0 ) {
        // can go up
        cells[i - 1][j][2] = cells[i][j][0];
      }
      if ( j >= 0 && j < 8 ) {
        cells[i][j + 1][3] =  cells[i][j][1];
      }
      if ( j <= 8 && j > 0 ) {
        cells[i][j - 1][1] =  cells[i][j][3];
      }
      if ( i > 0 && i < 4 && j > 0 && j < 8 ) {
        cells[i + 1][j][0] = cells[i][j][2];
        cells[i - 1][j][2] = cells[i][j][0];
        cells[i][j + 1][3] =  cells[i][j][1];
        cells[i][j - 1][1] =  cells[i][j][3];
      }
    }
    int getCellData(int i, int j, int k) {
      return cells[i][j][k];
    }
    int getStartX() {
      return startX;
    }
    int getStartY() {
      return startY;
    }
    void print() {
      for (int j = 0; j < cols; j++) {
        Serial.print(" ---"); // Print top walls
      }
      Serial.print("\n");
      for (int i = 0; i < rows; i++) {
        // Print Vertical;
        Serial.print("|"); // left most wall
        for (int j = 0; j < cols - 1; j++) {
          Serial.print(statusCells[i][j]);
          if (cells[i][j][1] == -1) {
            Serial.print("*");
          } else {
            if (cells[i][j][1] == 1) {
              Serial.print("|");
            } else {
              Serial.print(" ");
            }
          }
        }
        Serial.print(statusCells[i][cols - 1]);
        Serial.print("|"); // right most wall
        Serial.print("\n");
        if (i == 4) {
          break;
        }
        for (int j = 0; j < cols; j++) {
          if (cells[i][j][2] == -1) {
            Serial.print(" ***");
          } else {
            if (cells[i][j][2] == 1) {
              Serial.print(" ---");
            } else {
              Serial.print("    ");
            }
          }
        }
        Serial.print("\n");
      }
      for (int j = 0; j < cols; j++) {
        Serial.print(" ---"); // Print closing bottom walls
      }
      Serial.print("\n");
    }
    Head getHeading() {
      return head;
    }
};
#endif
