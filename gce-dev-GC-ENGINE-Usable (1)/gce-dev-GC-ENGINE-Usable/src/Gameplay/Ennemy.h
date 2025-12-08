#pragma once
#include "Entity.h"

class Ennemy :public Entity
{
	int Life;
public:
	Ennemy(GameObject* obj, float spd = 150);
	virtual ~Ennemy() = default;
	virtual void Die() = 0;
	virtual void Attack() = 0;
};