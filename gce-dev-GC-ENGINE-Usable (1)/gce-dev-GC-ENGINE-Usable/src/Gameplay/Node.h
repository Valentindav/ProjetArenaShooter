#pragma once
#include <vector>
#include <iostream>
#include <Engine.h>

class Agent;
struct Tile {
	int gridX = 0, gridY = 0;	
	bool walkable = true;

	float distanceToEnd = 0.f;
	float distanceToStart = 0.f;
	float totalCost = 0.f;
	bool way = false;
	
	float cost = 1.0f;
	gce::Vector2f32 worldPosition;
};

template<typename T>
struct Node {
	T* data;

	bool visited = false;
	Node* cameFrom = nullptr;

	std::vector<Node<Tile>*> neighbors;
};

struct CompareTile {
	bool operator()(Node<Tile>* a, Node<Tile>* b) {
		return a->data->distanceToStart > b->data->distanceToStart;
	}
};

struct CompareTileAStar {
	bool operator()(Node<Tile>* a, Node<Tile>* b) const {
		if (a->data->totalCost == b->data->totalCost) {
			if (a->data->gridX != b->data->gridX)
				return a->data->gridX > b->data->gridX;
			return a->data->gridY > b->data->gridY;
		}
		return a->data->totalCost > b->data->totalCost;
	}
};