#include "Entity.h"

DECLARE_SCRIPT(LifeScript, ScriptFlag::Update | ScriptFlag::CollisionEnter)
private:
    float life = 5.0f;

public:
    void Update()
    {
        GameObject* obj = m_pOwner;
    }

    void CollisionEnter(GameObject* other)
    {
        if (m_pOwner && m_pOwner->IsActive())
        {
            if (other->GetName() == "Bullet")
            {
                life = life - 1.0f;

                if (life <= 0.0f)
                {
                    m_pOwner->Destroy();
                }
            }
        }
    }
    END_SCRIPT

        Entity::Entity(GameObject* obj, float spd) : m_gameObject(obj), m_speed(spd)
    {
    }

    void Entity::AddLifeScript()
    {
        if (m_gameObject)
        {
            m_gameObject->AddScript<LifeScript>();
        }
    }