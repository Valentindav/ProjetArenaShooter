#pragma once
#include "Entity.h"

class Player :public Entity
{
public:
	float m_life = 10.0f;
	Player(GameObject* obj, float spd = 150);
	void AddMove();
};