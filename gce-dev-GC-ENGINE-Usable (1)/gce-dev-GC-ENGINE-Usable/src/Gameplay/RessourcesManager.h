#pragma once
#include "Engine.h"
#include "TileMap.h"

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
    static TileMap* GetTileMap();
	static void SetTileMap(TileMap* tileMap);

private:
    gce::Vector<Entity*> m_entities;
    inline static RessourcesManager* m_instance = nullptr;
    Player* m_player = nullptr;
    TileMap* m_tileMap;
};