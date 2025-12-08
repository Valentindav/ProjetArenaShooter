#include "RessourcesManager.h"
#include "Entity.h"

void RessourcesManager::Create()
{
    if (m_Instance != nullptr) return;
    m_Instance = new RessourcesManager();
}

void RessourcesManager::AddEntities(Entity* entity)
{
    if (entity == nullptr) return;
    if (m_Instance == nullptr) Create();
    m_Instance->m_entities.PushBack(entity);
}

gce::Vector<Entity*> RessourcesManager::getEntities()
{
    if (m_Instance == nullptr) return {};
    return m_Instance->m_entities;
}

Player* RessourcesManager::GetPlayer()
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->m_player;
}

void RessourcesManager::SetPlayer(Player* player)
{
    if (m_Instance == nullptr) Create();
    m_Instance->m_player = player;
}

gce::Geometry* RessourcesManager::GetBottle()
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->bulletGeo;
}

gce::Texture* RessourcesManager::GetTexture()
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->m_bottleTexture;
}

 Entity* RessourcesManager::GetEntityFromGameObject(gce::GameObject* go)
 {
     if (m_Instance == nullptr || go == nullptr) return nullptr;
     for (Entity* e : m_Instance->m_entities)
     {
         if (e && e->GetGameObject() == go) return e;
     }
     return nullptr;
 }