#pragma once
#include "Entity.h"

class Bullet :public Entity
{
	GameObject* m_owner = nullptr;
public:
	Bullet(GameObject* obj, float spd = 500);
	void AddShoot();
	void DeleteShoot();
	void SetTexture(std::string_view path);
	void SetShape(gce::Geometry* geo);
	void SetOwner(GameObject* owner) 
	{
		m_owner = owner;
	}
	GameObject* GetOwner() 
	{
		if (this == nullptr) return 0; return m_owner; }
};