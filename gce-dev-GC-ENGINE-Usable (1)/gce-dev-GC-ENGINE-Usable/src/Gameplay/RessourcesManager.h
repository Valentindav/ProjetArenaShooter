#pragma once
#include "Engine.h"

class Entity;
class Player;
class RessourcesManager
{
public:
    static void Create();
    static void AddEntities(Entity* entity);
    static gce::Vector<Entity*> getEntities();
    static Player* GetPlayer();
    static void SetPlayer(Player* player);

private:
    gce::Vector<Entity*> m_entities;
    inline static RessourcesManager* m_Instance = nullptr;
    Player* m_player = nullptr;
};