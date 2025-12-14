#pragma once
#include "Entity.h"

class Player;

class Bullet : public Entity
{
private:
    GameObject* m_owner;

public:
    Bullet(GameObject* obj, float spd = 2000) : Entity(obj, spd), m_owner(nullptr) {}

    void AddShoot();
    void DeleteShoot();

    void SetOwner(GameObject* owner)
    {
        m_owner = owner;
    }

    GameObject* GetOwner()
    {
        return m_owner;
    }
};