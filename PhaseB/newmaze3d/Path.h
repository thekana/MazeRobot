#ifndef PATH_H
#define PATH_H
#include "LinkedList.h"
#include "Maze.h"
#include <Arduino.h>
class Node {
  private:
    byte x;
    byte y;
    byte v;  //value
    byte t;
    Head from;
  public:
    Node(byte x, byte y, byte v, byte t, byte k) {
      this->x = x;
      this->y = y;
      this->v = v;
      this->t = t;
      this->from = (Head)k;
    }
    byte getX() {
      return x;
    }
    byte getY() {
      return y;
    }
    byte getValue() {
      return v;
    }
    byte getTurn() {
      return t;
    }
    Head getHead() {
      return from;
    }
    void print() {
      Serial.print("X ");
      Serial.print(x);
      Serial.print(" Y ");
      Serial.print(y);
      Serial.print(" Val ");
      Serial.print(v);
      Serial.print(" Head : ");
      Serial.println(from);
    }
};

class Path {
  public:
    byte turn = 0;
    LinkedList<Node*> path;
    Path() {
      path = LinkedList<Node*>();
    }
    void add(Node * n) {
      path.add(n);
    }
    void print() {
      for (byte i = 0; i < path.size(); i++) {
        path.get(i)->print();
      }
    }
};
#endif
