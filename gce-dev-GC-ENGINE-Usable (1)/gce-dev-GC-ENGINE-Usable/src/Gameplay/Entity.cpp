#include "Entity.h"
#include "RessourcesManager.h"

Entity::Entity(GameObject* obj, float spd) : m_gameObject(obj), m_speed(spd)
  {
      RessourcesManager::AddEntities(this);
  }

void Entity::TakeDamage(int damage) // Entity action when she take m_damage ->maybe override this with certain entity when needed
{
    if (this->m_life <= 0)
    {
        this->GetGameObject()->SetActive(false);
        this->GetGameObject()->Destroy();

        RessourcesManager::RemoveEntities(this);

        delete this;
    }
    else
    {
        this->m_life = this->m_life - damage;
		/*std::cout << "Entity took " << damage << " damage, remaining life: " << this->m_life << std::endl;*/
    }
}