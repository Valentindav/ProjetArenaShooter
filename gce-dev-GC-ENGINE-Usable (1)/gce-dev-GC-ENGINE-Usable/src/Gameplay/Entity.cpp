#include "Entity.h"
#include "RessourcesManager.h"

Entity::Entity(GameObject* obj, float spd) : m_gameObject(obj), m_speed(spd)
  {
      RessourcesManager::AddEntities(this);
  }

 void Entity::TakeDamage() // Entity action when she take damage ->maybe override this with certain entity when needed
   {
       if (this->m_life <= 0)
       {
            std::cout << "dead " << this->GetGameObject()->GetName() << std::endl;
            if (this->GetGameObject()->GetName() == "Player") {
                RessourcesManager::SetPlayer(nullptr);
            }
            this->GetGameObject()->SetActive(false);
            this->GetGameObject()->Destroy();
            delete this;
        }
        else
        {
            this->m_life = this->m_life - 1;
            std::cout << this->m_life << std::endl;
        }
    }