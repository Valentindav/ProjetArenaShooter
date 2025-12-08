#include "RessourcesManager.h"

void RessourcesManager::Create()
{
    if (m_instance != nullptr) return;
    m_instance = new RessourcesManager();
}

void RessourcesManager::AddEntities(Entity* entity)
{
    if (entity == nullptr) return;
    if (m_instance == nullptr) Create();
    m_instance->m_entities.PushBack(entity);
}

gce::Vector<Entity*> RessourcesManager::getEntities()
{
    if (m_instance == nullptr) return {};
    return m_instance->m_entities;
}

Player* RessourcesManager::GetPlayer()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_player;
}

void RessourcesManager::SetPlayer(Player* player)
{
    if (m_instance == nullptr) Create();
    m_instance->m_player = player;
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