#pragma once
#include "Ennemy.h"
#include "Bullet.h"

class Robot : public Ennemy
{
public:
	Bullet* m_laser = nullptr;
	float m_laserTimer = 1.0f;
	int m_laserPhase = 0;
	Vector3f32 m_lockedRotation;

	Robot(GameObject* obj, float spd = 50);
	void Die() override;
	void Attack()override;
	float m_waitCooldown = 2.f;
};

