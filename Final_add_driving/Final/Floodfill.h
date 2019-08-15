#ifndef FLOODFILL_H
#define FLOODFILL_H
#include "Maze.h"
#include <avr/pgmspace.h>
#define MAX_CELL_VALUE 50
class Floodfill
{
private:
  Maze *maze;
  byte cell[ROWS][COLS];
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
    for (byte i = 0; i < ROWS; i++)
    {
      for (byte j = 0; j < COLS; j++)
      {
        cell[i][j] = MAX_CELL_VALUE;
      }
    }
 
    cell[maze->goalI][maze->goalJ] = 0; // goal is always here
    byte currExploredValue = 0;
    byte mazeValueChange = 1;
    while (mazeValueChange != 0)
    {
      mazeValueChange = 0;
      for (byte i = 0; i < ROWS; i++)
      {
        for (byte j = 0; j < COLS; j++)
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
      wallsPathCount = cell[maze->getStartI()][maze->getStartJ()];
    }
    else
    {
      noWallsPathCount = cell[maze->getStartI()][maze->getStartJ()];
    }
  }
  byte incrementNeighbour(byte i, byte j, byte k, byte value)
  {
    switch (k)
    {
    case NORTH:
      j--;
      break;
    case SOUTH:
      j++;
      break;
    case EAST:
      i--;
      break;
    case WEST:
      i++;
      break;
    default:
      break;
    }
    if (cell[i][j] == MAX_CELL_VALUE)
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
    for (byte i = 0; i < ROWS; i++)
    {
      for (byte j = 0; j < COLS; j++)
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
    byte k = 4;
    do
    {
      k--;
      Serial.print(maze->hasWall(i, j, k));
    } while (k);
    Serial.print("\n");
    k = 4;
    do
    {
      k--;
      Serial.print(maze->isWalledExplored(i, j, k));
    } while (k);
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
