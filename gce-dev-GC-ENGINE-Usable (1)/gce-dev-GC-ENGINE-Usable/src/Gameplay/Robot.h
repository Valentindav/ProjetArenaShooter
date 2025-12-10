#pragma once
#include "Ennemy.h"
#include "Bullet.h"

class Robot : public Ennemy
{
public:
	Bullet* laser = nullptr;
	float laserTimer = 0.0f;
	int laserPhase = 0;
	Vector3f32 lockedRotation;

	Robot(GameObject* obj, float spd = 50);
	void Die() override;
	void Attack()override;
	float m_WaitCooldown = 2.f;
	float m_laserTimer = 1.f;
};

