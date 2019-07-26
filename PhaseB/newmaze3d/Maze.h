#ifndef MAZE_H
#define MAZE_H
#include <Arduino.h>
#include <avr/pgmspace.h>

#define ROWS 5
#define COLS 9

byte x2i(char *s)
{
  byte x = 0;
  for (;;)
  {
    char c = *s;
    if (c >= '0' && c <= '9')
    {
      x *= 16;
      x += c - '0';
    }
    else if (c >= 'A' && c <= 'F')
    {
      x *= 16;
      x += (c - 'A') + 10;
    }
    else
      break;
    s++;
  }
  return x;
}
enum Heading
{
  EAST = 3,
  SOUTH = 2,
  WEST = 1,
  NORTH = 0
};

class Maze
{
  private:
    byte cells[ROWS][COLS] = {{0}};
    byte exploredWalls[ROWS][COLS] = {{0}};
    String statusCells[ROWS][COLS];
    byte startI = 0;
    byte startJ = 0;
    Heading head;

  public:
    Maze()
    {
      initializeCells();         // 2 is unexplored
      initializeStatusCells();   // 4 spaces
      statusCells[2][4] = " X "; // mark goal
    }
    void initializeCells()
    {
      for (byte i = 0; i < 5; i++)
      {
        for (byte j = 0; j < 9; j++)
        {
          if (i == 0)
          {
            cells[i][j] |= (1 << EAST);
            exploredWalls[i][j] |= (1 << EAST);
          }
          if (i == ROWS - 1)
          {
            cells[i][j] |= (1 << WEST);
            exploredWalls[i][j] |= (1 << WEST);
          }
          if (j == 0)
          {
            cells[i][j] |= (1 << NORTH);
            exploredWalls[i][j] |= (1 << NORTH);
          }
          if (j == COLS - 1)
          {
            cells[i][j] |= (1 << SOUTH);
            exploredWalls[i][j] |= (1 << SOUTH);
          }
        }
      }
    }

    void markAsExplored() {
      for (byte i = 0; i < 5; i++)
      {
        for (byte j = 0; j < 9; j++)
        {
          // set every cells to 4 spaces
          exploredWalls[i][j] = 0xF;
        }
      }
    }
    // reseting maze
    void initializeStatusCells()
    {
      for (byte i = 0; i < 5; i++)
      {
        for (byte j = 0; j < 9; j++)
        {
          // set every cells to 4 spaces
          statusCells[i][j] = "   ";
        }
      }
    }
    void updateStatusCells(byte i, byte j, String const &h)
    {
      // to update these cells with path
      // data and start/end
      initializeStatusCells();
      statusCells[2][4] = " X ";
      if (h == "E")
      {
        head = EAST;
      }
      else if (h == "S")
      {
        head = SOUTH;
      }
      else if (h == "W")
      {
        head = WEST;
      }
      else if (h == "N")
      {
        head = NORTH;
      }
      statusCells[startI][startJ] = "   ";
      Serial.println("hey");
      statusCells[i][j] = String(" " + h + " ");
      startI = i;
      startJ = j;
    }
    void addPath(byte i, byte j, byte val)
    {
      String space = " ";
      String str = String(val);
      space.concat(str);
      if (val < 10)
      {
        space.concat(" ");
      }
      statusCells[i][j] = space;
    }
    void fillCells(String const &h)
    {
      byte index = 0;
      byte len = h.length();
      while (index < len)
      {
        byte i = h.charAt(index) - '0';
        index++;
        byte j = h.charAt(index) - '0';
        index++;
        char a = h.charAt(index);
        byte data = x2i(&a);
        index++;
        // Now update the cell data
        // byte data = decimal * 10 + digit;
        // BITMASK E-S-W-N
        //        cells[i][j] |= (data & B1000); // EAST
        //        cells[i][j] |= (data & B0100); // SOUTH
        //        cells[i][j] |= (data & B0010); // WEST
        //        cells[i][j] |= (data & B0001); // NORTH
        cells[i][j] = data;
        // Also need to mark cells as explored
        exploredWalls[i][j] = 0x0F;
        updateNeighbours(i, j);
      }
    }
    void updateNorth(byte i, byte j, byte n) {
      cells[i][j] |= n << NORTH;
      exploredWalls[i][j] |= 1 << NORTH;
      updateNeighbours(i, j);
    }
    void updateEast(byte i, byte j, byte n) {
      cells[i][j] |= n << EAST;
      exploredWalls[i][j] |= 1 << EAST;
      updateNeighbours(i, j);
    }
    void updateWest(byte i, byte j, byte n) {
      cells[i][j] |= n << WEST;
      exploredWalls[i][j] |= 1 << WEST;
      updateNeighbours(i, j);
    }
    void updateSouth(byte i, byte j, byte n) {
      cells[i][j] |= n << SOUTH;
      exploredWalls[i][j] |= 1 << SOUTH;
      updateNeighbours(i, j);
    }
    void updateNeighbours(byte i, byte j)
    {
      if (i >= 0 && i < 4)
      {
        // EAST <--> WEST
        // cells[i + 1][j] |= ((cells[i][j] & (1 << WEST)) << 2);
        if (isWalledExplored(i, j, WEST)) {
          (cells[i][j] & (1 << WEST)) ? setBit(i + 1, j, EAST) : clearBit(i + 1, j, EAST);
          exploredWalls[i + 1][j] |= (1 << EAST);
        }
      }
      if (i <= 4 && i > 0)
      {
        // WEST <--> EAST
        // cells[i - 1][j] |= ((cells[i][j] & (1 << EAST)) >> 2);
        if (isWalledExplored(i, j, EAST)) {
          (cells[i][j] & (1 << EAST)) ? setBit(i - 1, j, WEST) : clearBit(i - 1, j, WEST);
          exploredWalls[i - 1][j] |= (1 << WEST);
        }
      }
      if (j >= 0 && j < 8)
      {
        // NORTH <--> SOUTH
        // cells[i][j + 1] |= ((cells[i][j] & (1 << SOUTH)) >> 2);
        if (isWalledExplored(i, j, SOUTH)) {
          (cells[i][j] & (1 << SOUTH)) ? setBit(i, j + 1, NORTH) : clearBit(i, j + 1, NORTH);
          exploredWalls[i][j + 1] |= (1 << NORTH);
        }
      }
      if (j <= 8 && j > 0)
      {
        // SOUTH <--> NORTH
        // cells[i][j - 1] |= ((cells[i][j] & (1 << NORTH)) << 2);
        if (isWalledExplored(i, j, NORTH)) {
          (cells[i][j] & (1 << NORTH)) ? setBit(i, j - 1, SOUTH) : clearBit(i, j - 1, SOUTH);
          exploredWalls[i][j - 1] |= (1 << SOUTH);
        }
      }
    }
    void setBit(byte i, byte j, byte n) {
      cells[i][j] |= 1 << n;
    }

    void clearBit(byte i, byte j, byte n) {
      cells[i][j] &= ~(1 << n);
    }
    byte getStartI()
    {
      return startI;
    }
    byte getStartJ()
    {
      return startJ;
    }
    void print()
    {
      for (byte j = 0; j < COLS; j++)
      {
        Serial.print(F(" ---")); // Print top walls
      }
      Serial.print("\n");
      for (byte i = 0; i < ROWS; i++)
      {
        // Print Vertical;
        Serial.print("|"); // left most wall
        for (byte j = 0; j < COLS - 1; j++)
        {
          Serial.print(statusCells[i][j]);
          // if south wall not explored
          if (!(exploredWalls[i][j] & (1 << SOUTH)))
          {
            Serial.print("*");
          }
          else
          {
            if (hasWall(i, j, SOUTH))
            {
              Serial.print("|");
            }
            else
            {
              Serial.print(" ");
            }
          }
        }
        Serial.print(statusCells[i][COLS - 1]);
        Serial.print("|"); // right most wall
        Serial.print("\n");
        if (i == 4)
        {
          break;
        }
        for (byte j = 0; j < COLS; j++)
        {
          if (!(exploredWalls[i][j] & (1 << WEST)))
          {
            Serial.print(F(" ***"));
          }
          else
          {
            if (hasWall(i, j, WEST))
            {
              Serial.print(F(" ---"));
            }
            else
            {
              Serial.print(F("    "));
            }
          }
        }
        Serial.print("\n");
      }
      for (byte j = 0; j < COLS; j++)
      {
        Serial.print(F(" ---")); // Print closing bottom walls
      }
      Serial.print("\n");
    }
    Heading getHeading()
    {
      return head;
    }
    boolean hasWall(int i, int j, int h)
    {
      // return true if the wall exists
      return (cells[i][j] & (1 << h));
    }
    boolean isWalledExplored(int i, int j, int h)
    {
      // return true if the wall has been explored
      return (exploredWalls[i][j] & (1 << h));
    }
};
#endif
