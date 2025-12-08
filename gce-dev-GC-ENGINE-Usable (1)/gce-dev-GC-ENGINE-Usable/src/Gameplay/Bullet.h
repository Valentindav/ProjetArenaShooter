#pragma once
#include "Entity.h"

class Bullet :public Entity
{
	GameObject* m_owner = nullptr;
public:
	Bullet(GameObject* obj, float spd = 500) : Entity(obj, spd) { }
	void AddShoot();
	void DeleteShoot();
	void SetOwner(GameObject* owner) { m_owner = owner; }
	GameObject* GetOwner() { if (this == nullptr) return 0; return m_owner; }
};