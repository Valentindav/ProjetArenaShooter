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

void RessourcesManager::RemoveEntities(Entity* entity) // supprime l'entity du vecteur sans la delete
{
    if (entity == nullptr || m_instance == nullptr) return;
    for (auto it = m_instance->m_entities.begin(); it != m_instance->m_entities.end(); ++it)
    {
        if (*it == entity)
        {
            m_instance->m_entities.Erase(it);
            return;
        }
    }
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

void RessourcesManager::SetEnemySelected(Entity* enemy) // set selected enemy
{
    if (m_instance == nullptr) Create();
	m_instance->m_enemySelected = enemy;
}

gce::Geometry* RessourcesManager::GetBottle() // get bottle obj
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->bulletGeo;
}

gce::Geometry* RessourcesManager::GetThomson()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_thomsonGeo;
}

gce::Geometry* RessourcesManager::GetsurgarCane()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_surgarCaneGeo;
}

gce::Geometry* RessourcesManager::GetNerf()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_nerfGeo;
}

gce::Geometry* RessourcesManager::GetElf() {
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_elfGeo;
}

gce::Geometry* RessourcesManager::GetSnowMan() {
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_snowManGeo;
}

gce::Geometry* RessourcesManager::GetRobot() {
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_robotGeo;
}

gce::Geometry* RessourcesManager::GetDeer(){
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_deerGeo;
}

gce::Geometry* RessourcesManager::GetSanta() {
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_SantaGeo;
}

gce::Texture* RessourcesManager::GetTexture() // get wall texture -> need rename & thing
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_bottleTexture;
}

Entity* RessourcesManager::GetSelectedEnemy() // get selected enemy
{
    if (m_instance == nullptr) return nullptr;
	return m_instance->m_enemySelected;
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