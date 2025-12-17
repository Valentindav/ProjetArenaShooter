#pragma once
#include "Ennemy.h"
#include "TileMap.h"
#include <vector>

using namespace gce;

class Heal : public Entity
{
private:
	GameObject* m_healObject = nullptr;

public:
	Heal(GameObject* obj, float spd = 150);
	void Die() override;
	void AddScript();
};

