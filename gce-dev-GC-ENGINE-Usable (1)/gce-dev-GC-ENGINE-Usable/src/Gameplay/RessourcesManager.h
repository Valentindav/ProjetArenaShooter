#pragma once
#include "Engine.h"

class Entity;
class Player;
class SnowMan;
class RessourcesManager
{
public:
    static void Create();
    static void AddEntities(Entity* entity);
    static gce::Vector<Entity*> getEntities();
    static Player* GetPlayer();
    static SnowMan* GetSnowMan();
    static void SetPlayer(Player* player);
    static void SetSnowMan(SnowMan* snowman);

private:
    gce::Vector<Entity*> m_entities;
    inline static RessourcesManager* m_Instance = nullptr;
    Player* m_player = nullptr;
    SnowMan* m_snowman = nullptr;
};