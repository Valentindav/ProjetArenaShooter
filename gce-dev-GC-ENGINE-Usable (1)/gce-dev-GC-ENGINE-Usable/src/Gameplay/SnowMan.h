#pragma once
#include "Ennemy.h"

class SnowMan : public Ennemy
{
public : 
	float m_life = 10.0f;
	SnowMan(GameObject* obj, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
	float m_ShootCooldown = 0.75f;
};

