#ifndef MAZE_H
#define MAZE_H
#include <Arduino.h>
#include <avr/pgmspace.h>

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
  byte cells[5][9] = {{0}};
  byte exploredWalls[5][9] = {{0}};
  String statusCells[5][9];
  byte rows = 5;
  byte cols = 9;
  byte startX = 0;
  byte startY = 0;
  Heading head;

public:
  Maze(String const &h)
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
        }
        if (i == 4)
        {
          cells[i][j] |= (1 << WEST);
        }
        if (j == 0)
        {
          cells[i][j] |= (1 << NORTH);
        }
        if (j == 8)
        {
          cells[i][j] |= (1 << SOUTH);
        }
      }
    }
  }
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
    statusCells[i][j] = String(" " + h + " ");
    startX = i;
    startY = j;
    //      Serial.print("Updated");
    //      Serial.print(i); Serial.print(j); Serial.println(statusCells[i][j]);
  }
  void addPath(byte i, byte j, byte val)
  {
    Serial.println(val);
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
      cells[i][j] |= (data & B1000); // EAST
      cells[i][j] |= (data & B0100); // SOUTH
      cells[i][j] |= (data & B0010); // WEST
      cells[i][j] |= (data & B0001); // NORTH
      // Also need to mark cells as explored
      exploredWalls[i][j] = 0x0F;
      updateNeighbours(i, j);
    }
  }
  void updateNeighbours(byte i, byte j)
  {
    if (i >= 0 && i < 4)
    {
      // EAST <--> WEST
      cells[i + 1][j] |= ((cells[i][j] & (1 << WEST)) << 2);
      exploredWalls[i + 1][j] |= (1 << 3);
      //cells[i + 1][j][0] = cells[i][j][2];
    }
    if (i <= 4 && i > 0)
    {
      // WEST <--> EAST
      cells[i - 1][j] |= ((cells[i][j] & (1 << EAST)) >> 2);
      exploredWalls[i - 1][j] |= (1 << 1);
      //cells[i - 1][j][2] = cells[i][j][0];
    }
    if (j >= 0 && j < 8)
    {
      // NORTH <--> SOUTH
      cells[i][j + 1] |= ((cells[i][j] & (1 << SOUTH)) >> 2);
      exploredWalls[i][j + 1] |= (1 << 0);
      //cells[i][j + 1][3] = cells[i][j][1];
    }
    if (j <= 8 && j > 0)
    {
      // SOUTH <--> NORTH
      cells[i][j - 1] |= ((cells[i][j] & (1 << NORTH)) << 2);
      exploredWalls[i][j - 1] |= (1 << 2);
      //cells[i][j - 1][1] = cells[i][j][3];
    }
    if (i > 0 && i < 4 && j > 0 && j < 8)
    {
      cells[i + 1][j] |= ((cells[i][j] & (1 << WEST)) << 2);
      cells[i - 1][j] |= ((cells[i][j] & (1 << EAST)) >> 2);
      cells[i][j + 1] |= ((cells[i][j] & (1 << SOUTH)) >> 2);
      cells[i][j - 1] |= ((cells[i][j] & (1 << NORTH)) << 2);
      exploredWalls[i + 1][j] |= (1 << EAST);
      exploredWalls[i - 1][j] |= (1 << WEST);
      exploredWalls[i][j + 1] |= (1 << NORTH);
      exploredWalls[i][j - 1] |= (1 << SOUTH);
      // cells[i + 1][j][0] = cells[i][j][2];
      // cells[i - 1][j][2] = cells[i][j][0];
      // cells[i][j + 1][3] = cells[i][j][1];
      // cells[i][j - 1][1] = cells[i][j][3];
    }
  }
  byte getCellData(byte i, byte j, byte k)
  {
    return 0;
  }
  byte getStartX()
  {
    return startX;
  }
  byte getStartY()
  {
    return startY;
  }
  void print()
  {
    for (byte j = 0; j < cols; j++)
    {
      Serial.print(F(" ---")); // Print top walls
    }
    Serial.print("\n");
    for (byte i = 0; i < rows; i++)
    {
      // Print Vertical;
      Serial.print("|"); // left most wall
      for (byte j = 0; j < cols - 1; j++)
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
      Serial.print(statusCells[i][cols - 1]);
      Serial.print("|"); // right most wall
      Serial.print("\n");
      if (i == 4)
      {
        break;
      }
      for (byte j = 0; j < cols; j++)
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
    for (byte j = 0; j < cols; j++)
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
    return (cells[i][j] & (1 << h));
  }
};
#endif
