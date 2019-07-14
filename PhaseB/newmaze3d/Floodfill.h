#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "Maze.h"
#include <avr/pgmspace.h>
class Floodfill
{
private:
  Maze *maze;
  byte cell[5][9];
  byte rows = 5;
  byte cols = 9;
  byte noWallsPathCount;
  byte wallsPathCount;
  bool wallAssumption = true;

public:
  Floodfill(Maze *maze = nullptr)
  {
    this->maze = maze;
  }
  void AssumeNoWalls()
  {
    this->wallAssumption = false;
  }
  void AssumeWalls()
  {
    this->wallAssumption = true;
  }
  void doFloodfill()
  {
    for (byte i = 0; i < rows; i++)
    {
      for (byte j = 0; j < cols; j++)
      {
        cell[i][j] = 50;
      }
    }
    cell[2][4] = 0;
    byte currExploredValue = 0;
    byte mazeValueChange = 1;
    while (mazeValueChange != 0)
    {
      mazeValueChange = 0;
      for (byte i = 0; i < rows; i++)
      {
        for (byte j = 0; j < cols; j++)
        {
          if (cell[i][j] == currExploredValue)
          {
            for (byte k = 0; k < 4; k++)
            {
              // Check unexplored wall assumption
              if (!maze->isWalledExplored(i, j, k))
              {
                // unexplored wall assume no walls
                if (!wallAssumption)
                {
                  // do update
                  mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
                }
              }
              // The wall is explored, now check if there is a wall.
              // If no wall, we increment its neighbours
              else if (maze->hasWall(i, j, k) == 0)
              {
                mazeValueChange = mazeValueChange | incrementNeighbour(i, j, k, cell[i][j]);
              }
            }
          }
        }
      }
      currExploredValue++;
    }
    if (wallAssumption)
    {
      wallsPathCount = cell[maze->getStartX()][maze->getStartY()];
    }
    else
    {
      noWallsPathCount = cell[maze->getStartX()][maze->getStartY()];
    }
  }
  byte incrementNeighbour(byte i, byte j, byte k, byte value)
  {
    switch (k)
    {
    case NORTH:
      j = j - 1;
      break;
    case SOUTH:
      j = j + 1;
      break;
    case EAST:
      i = i - 1;
      break;
    case WEST:
      i = i + 1;
      break;
    default:
      break;
    }
    if (cell[i][j] == 50)
    {
      cell[i][j] = value + 1;
      return 1;
    }
    else
    {
      return 0;
    }
  }
  void print()
  {
    for (byte i = 0; i < rows; i++)
    {
      for (byte j = 0; j < cols; j++)
      {
        Serial.print(cell[i][j]);
        Serial.print("\t");
      }
      Serial.print("\n");
    }
    Serial.print("\n");
  }
  void printCell(byte i, byte j)
  {
    for (byte k = 0; k < 4; k++)
    {
      Serial.print(maze->hasWall(i, j, k));
    }
    Serial.print("\n");
  }
  bool sufficientlyExplored()
  {
    return wallsPathCount == noWallsPathCount;
  }
  byte getCell(byte i, byte j)
  {
    return cell[i][j];
  }
};
#endif
