#pragma once
#include "Entity.h"
class Player :public Entity
{
public:
	Player(GameObject* obj, float spd = 150) : Entity(obj, spd) {}
	void MovePlayer();
	void AddMove();
};