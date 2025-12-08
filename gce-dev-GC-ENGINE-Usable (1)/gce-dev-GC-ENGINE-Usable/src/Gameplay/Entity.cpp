#include "Entity.h"

DECLARE_SCRIPT(LifeScript, ScriptFlag::Update)
private:


public:


    void Update()
    {
        GameObject* obj = m_pOwner;
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