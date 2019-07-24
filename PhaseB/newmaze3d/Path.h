#ifndef PATH_H
#define PATH_H
#include "LinkedList.h"
#include "Maze.h"
#include <Arduino.h>
#include <avr/pgmspace.h>
class Node
{
private:
  byte x;
  byte y;
  byte v; //value
  Heading from;

public:
  Node(byte x, byte y, byte v, byte k)
  {
    this->x = x;
    this->y = y;
    this->v = v;
    this->from = (Heading)k;
  }
  byte getX()
  {
    return x;
  }
  byte getY()
  {
    return y;
  }
  byte getValue()
  {
    return v;
  }
  Heading getHead()
  {
    return from;
  }
  void print()
  {
    Serial.print(F("ROW "));
    Serial.print(x);
    Serial.print(F(" COL "));
    Serial.print(y);
    Serial.print(F(" Val "));
    Serial.print(v);
    Serial.print(F(" Head : "));
    Serial.println(from);
  }
};

class Path
{
public:
  byte turn = 0;
  LinkedList<Node *> path;
  Path()
  {
    path = LinkedList<Node *>();
  }
  void add(Node *n)
  {
    path.add(n);
  }
  void print()
  {
    for (byte i = 0; i < path.size(); i++)
    {
      path.get(i)->print();
    }
  }
};
#endif
