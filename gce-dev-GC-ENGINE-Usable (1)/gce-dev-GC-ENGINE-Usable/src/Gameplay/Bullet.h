#pragma once
#include "Entity.h"

class Bullet :public Entity
{
	GameObject* m_owner = nullptr;
	int m_damage = 1;
	float m_lifeTime = 0.0f;
public:
	Bullet(GameObject* obj, float spd = 500);
	void AddShoot();
	void DeleteShoot();
	void SetTexture(std::string_view path);
	void SetShape(gce::Geometry* geo);
	void SetDamage(int dmg);
	int GetDamage() const { return m_damage; }
	void SetLifeTime(float time) { m_lifeTime = time; }
	float GetLifeTime() const { return m_lifeTime; }
	void SetOwner(GameObject* owner) 
	{
		m_owner = owner;
	}
	GameObject* GetOwner() 
	{
		if (this == nullptr) return 0; return m_owner; }
};