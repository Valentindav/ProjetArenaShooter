#pragma once
#include "Entity.h"
#define SHOOT_TIMER_WAIT 0.0f

class Player :public Entity
{
public:
	float m_energy = 100.0f;
	Player(GameObject* obj, float spd = 150);
	void AddMove();
};