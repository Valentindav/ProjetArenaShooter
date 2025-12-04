#pragma once
#include "Entity.h"

class Player :public Entity
{
public:
	Player(GameObject* obj, float spd = 150);
	void MovePlayer();
	void AddMove();
};