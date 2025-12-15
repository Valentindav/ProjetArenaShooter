#include "RessourcesManager.h"
#include "Entity.h"

void RessourcesManager::Create()
{
    if (m_instance != nullptr) return;
    m_instance = new RessourcesManager();
}

void RessourcesManager::AddEntities(Entity* entity) // add entities to entity vector -> done at each entity created
{
    if (entity == nullptr) return;
    if (m_instance == nullptr) Create();
    m_instance->m_entities.PushBack(entity);
}

gce::Vector<Entity*> RessourcesManager::getEntities() // get entities vector
{
    if (m_instance == nullptr) return {};
    return m_instance->m_entities;
}

Player* RessourcesManager::GetPlayer() // get player entity
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_player;
}

void RessourcesManager::SetPlayer(Player* player) // set player entity
{
    if (m_instance == nullptr) Create();
    m_instance->m_player = player;
}

gce::Geometry* RessourcesManager::GetBottle() // get bottle obj
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->bulletGeo;
}

gce::Texture* RessourcesManager::GetTexture() // get wall texture -> need rename & thing
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_bottleTexture;
}

 Entity* RessourcesManager::GetEntityFromGameObject(gce::GameObject* go) // use game object to get entity
 {
     if (m_instance == nullptr || go == nullptr) return nullptr;
     for (Entity* e : m_instance->m_entities)
     {
         if (e->GetGameObject() == go) return e;
     }
     return nullptr;
 }
 
TileMap* RessourcesManager::GetTileMap()
{
    if (m_instance == nullptr) return {};
    return m_instance->m_tileMap;
}

void RessourcesManager::SetTileMap(TileMap* tileMap)
{
    if (m_instance == nullptr) Create();
    m_instance->m_tileMap = tileMap;
}

void RessourcesManager::AddLevelObject(GameObject* obj)
{
    if (obj)
    {
        m_levelObjects.PushBack(obj);
    }
}

void RessourcesManager::ClearCurrentLevel()
{
    for (GameObject* obj : m_levelObjects)
    {
        if (obj != nullptr)
        {
			obj->Destroy();
        }
    }
    m_levelObjects.Clear();
}