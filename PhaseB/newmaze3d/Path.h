#ifndef PATH_H
#define PATH_H
#include "LinkedList.h"
#include "Maze.h"
#include <Arduino.h>
class Node {
private:
	int x;
	int y;
	int v;  //value
	int t;
	Head from;
public:
	Node(int x, int y, int v, int t, int k) {
		this->x = x;
		this->y = y;
		this->v = v;
		this->t = t;
		this->from = (Head)k;
	}
	int getX() {
		return x;
	}
	int getY() {
		return y;
	}
	int getValue() {
		return v;
	}
	int getTurn() {
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
	int turn = 0;
	LinkedList<Node*> path;
	Path() {
		path = LinkedList<Node*>();
	}
	void add(Node * n) {
		path.add(n);
	}
	void print() {
		for (int i = 0; i < path.size(); i++) {
			path.get(i)->print();
		}
	}
};
#endif
