#pragma once
#include <vector>
#include <iostream>

class Agent;
struct Tile {
	int gridX = 0, gridY = 0, gridZ = 0;
	float X = 0.f, Y = 0.f, Z = 0.f;
	bool Walkable = true;

	float DistanceToEnd = 0.f;
	float DistanceToStart = 0.f;
	float totalCost = 0.f;
	bool Way = false;
	
	float Cost = 1.0f;
};

template<typename T>
struct Node {
	T* Data;

	bool Visited = false;
	Node* CameFrom = nullptr;

	std::vector<Node<Tile>*> neighbors;
};

struct CompareTile {
	bool operator()(Node<Tile>* a, Node<Tile>* b) {
		return a->Data->DistanceToStart > b->Data->DistanceToStart;
	}
};

struct CompareTileAStar {
	bool operator()(Node<Tile>* a, Node<Tile>* b) const {
		if (a->Data->totalCost == b->Data->totalCost) {
			if (a->Data->gridX != b->Data->gridX)
				return a->Data->gridX > b->Data->gridX;
			return a->Data->gridY > b->Data->gridY;
		}
		return a->Data->totalCost > b->Data->totalCost;
	}
};