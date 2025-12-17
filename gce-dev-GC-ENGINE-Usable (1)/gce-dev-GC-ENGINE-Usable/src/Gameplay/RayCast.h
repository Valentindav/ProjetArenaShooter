#pragma once
#include <Engine.h>
#include "RessourcesManager.h"

class RayCast
{
private:
	GameObject* m_rayCastObject;
	float32 m_distanceToEnemy = 20.f;
	float32 m_offsetFromPlayer;
public:
	RayCast(GameObject* obj, float32 offset);
	GameObject* GetRayCastObject() const { return m_rayCastObject; }
	void AddSelectEnemy();
};