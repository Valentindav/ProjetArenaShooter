#pragma once
#include "Entity.h"

class Bullet :public Entity
{

public:
	Bullet(GameObject* obj, float spd = 150) : Entity(obj, spd) { }
	void AddShoot();
	void DeleteShoot();
};