#include "Entity.h"

DECLARE_SCRIPT(Move, ScriptFlag::Update | ScriptFlag::CollisionStay)
private:
    float life = 5.0f;
public:
    void Update()
    {
        GameObject* obj = m_pOwner;
    }


void Life(GameObject* other)
    {
        if (m_pOwner && m_pOwner->IsActive())
        {
            life = life - 1;
            if (life <= 0)
            {
                Destroy();
            }
        }
    }

END_SCRIPT


Entity::Entity(GameObject* obj, float spd ) : m_gameObject(obj), m_speed(spd) 
{
}

