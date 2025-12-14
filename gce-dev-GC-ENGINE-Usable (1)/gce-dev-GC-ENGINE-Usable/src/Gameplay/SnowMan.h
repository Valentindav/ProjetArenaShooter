#pragma once
#include "Ennemy.h"

class SnowMan : public Ennemy
{
public:
    float m_life = 5.0f;

    SnowMan(GameObject* obj, float spd = 1.0f);
    void Die();
    void Attack();
    void AddScript();
};