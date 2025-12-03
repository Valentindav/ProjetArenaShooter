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
	void SetSpeed(float spd) { m_speed = spd; }
	float GetSpeed() const { return m_speed; }
	GameObject* GetGameObject() const { return m_gameObject; }	
};