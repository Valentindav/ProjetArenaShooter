#pragma once
#include "Ennemy.h"

class SnowMan : public Ennemy
{
public : 
	SnowMan(GameObject* obj, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
};

