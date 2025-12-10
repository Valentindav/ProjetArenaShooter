#pragma once
#include <Engine.h>

using namespace gce;

class Entity
{
protected:
    GameObject* m_gameObject;
public:
    float m_speed;

    float m_life = 10;
    Entity(GameObject* obj, float spd = 1.0f);
    virtual ~Entity() = default;

    GameObject* GetGameObject() 
    {
        return m_gameObject; 
    }

    float GetSpeed() const 
    {
        return m_speed;
    }
    virtual void Die() {}
    virtual void TakeDamage(int damage);
};