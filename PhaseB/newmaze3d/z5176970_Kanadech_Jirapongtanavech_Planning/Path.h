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
  byte actionCount = 0;
  byte completed = 0;
  LinkedList<Node *> *nodeList;
  Path()
  {
    nodeList = new LinkedList<Node *>();
  }
  ~Path()
  {
    delete nodeList;
  }
  void add(Node *n)
  {
    nodeList->add(n);
  }
  void print()
  {
    for (byte i = 0; i < nodeList->size(); i++)
    {
      nodeList->get(i)->print();
    }
  }
  byte getLastX()
  {
    return nodeList->get(nodeList->size() - 1)->getX();
  }
  byte getLastY()
  {
    return nodeList->get(nodeList->size() - 1)->getY();
  }
  byte getLastValue()
  {
    return nodeList->get(nodeList->size() - 1)->getValue();
  }
  byte getLastHeading()
  {
    return nodeList->get(nodeList->size() - 1)->getHead();
  }
  void clearNodeList()
  {
    while (nodeList->size() > 0)
    {
      Node *toDel = nodeList->pop();
      if (toDel)
      {
        delete toDel;
      }
    }
    nodeList->clear();
  }
};
#endif
