#pragma once
#include <Engine.h>

using namespace gce;

class Entity {
protected:
    GameObject* m_gameObject;
    float m_speed;

public:
    Entity(GameObject* obj, float spd = 1.0f);
    virtual ~Entity() = default;

    GameObject* GetGameObject() { return m_gameObject; }
    float GetSpeed() const { return m_speed; }

    void AddLifeScript();
    virtual void Die() {}
};