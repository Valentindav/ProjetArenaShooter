#include "RessourcesManager.h"
#include "Entity.h"

void RessourcesManager::Create()
{
    if (m_Instance != nullptr) return;
    m_Instance = new RessourcesManager();
}

void RessourcesManager::AddEntities(Entity* entity) // add entities to entity vector -> done at each entity created
{
    if (entity == nullptr) return;
    if (m_Instance == nullptr) Create();
    m_Instance->m_entities.PushBack(entity);
}

gce::Vector<Entity*> RessourcesManager::getEntities() // get entities vector
{
    if (m_Instance == nullptr) return {};
    return m_Instance->m_entities;
}

Player* RessourcesManager::GetPlayer() // get player entity
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->m_player;
}

void RessourcesManager::SetPlayer(Player* player) // set player entity
{
    if (m_Instance == nullptr) Create();
    m_Instance->m_player = player;
}

gce::Geometry* RessourcesManager::GetBottle() // get bottle obj
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->bulletGeo;
}

gce::Texture* RessourcesManager::GetTexture() // get wall texture -> need rename & thing
{
    if (m_Instance == nullptr) return nullptr;
    return m_Instance->m_bottleTexture;
}

 Entity* RessourcesManager::GetEntityFromGameObject(gce::GameObject* go) // use game object to get entity
 {
     if (m_Instance == nullptr || go == nullptr) return nullptr;
     for (Entity* e : m_Instance->m_entities)
     {
         if (e->GetGameObject() == go) return e;
     }
     return nullptr;
 }
