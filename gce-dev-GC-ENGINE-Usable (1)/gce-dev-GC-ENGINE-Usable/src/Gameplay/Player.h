#pragma once
#include "Entity.h"
#define SHOOT_TIMER_WAIT 0.0f

class Player :public Entity
{
public:
	Player(GameObject* obj, float spd = 150);
	void AddMove();
};