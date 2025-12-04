#pragma once
#include "Engine.h"
using namespace gce;

class Entity
{
private:
	float m_speed;
	GameObject* m_gameObject;
public:
	 Entity(GameObject* obj, float spd = 100);
	 virtual void SetSpeed(float spd) { m_speed = spd; }
	 virtual float GetSpeed() const { return m_speed; }
	 virtual GameObject* GetGameObject() const { return m_gameObject; }
};