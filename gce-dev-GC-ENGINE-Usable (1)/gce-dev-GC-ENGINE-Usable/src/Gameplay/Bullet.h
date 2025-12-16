#pragma once
#include "Entity.h"

class Bullet : public Entity
{
	GameObject* m_owner;
	int m_damage;
	float m_lifeTime;

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
	void SetOwner(GameObject* owner) { m_owner = owner; }
	GameObject* GetOwner() { if (this == nullptr) return nullptr; return m_owner; }
};