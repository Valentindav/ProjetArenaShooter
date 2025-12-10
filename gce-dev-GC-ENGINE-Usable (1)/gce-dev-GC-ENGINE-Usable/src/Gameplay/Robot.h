#pragma once
#include "Ennemy.h"

class Robot : public Ennemy
{
public:

	Robot(GameObject* obj, float spd = 50);
	void Die() override;
	void Attack()override;
	float m_WaitCooldown = 2.f;
	float m_laserTimer = 1.f;
};

