#pragma once
#include <Engine.h>
#include "TileMap.h"
#include "JsonImporter.hpp"

class Entity;
class Player;
class Ennemy;
class RessourcesManager
{
public:
    static void Create();
    static void AddEntities(Entity* entity);
    static void RemoveEntities(Entity* entity);
    static gce::Vector<Entity*> getEntities();
    static Player* GetPlayer();
    static void SetPlayer(Player* player);
	static void SetEnemySelected(Entity* enemy);
    static void AddLevel(ImportedLevelData level);
    static void ClearCurrentLevel();
    
    static gce::Geometry* GetBottle();
    static gce::Geometry* GetThomson();
    static gce::Geometry* GetsurgarCane();
    static gce::Geometry* GetNerf();

    static gce::Geometry* GetElf();
    static gce::Geometry* GetSnowMan();
    static gce::Geometry* GetRobot();
    static gce::Geometry* GetDeer();
    static gce::Geometry* GetSanta();

    static gce::Texture* GetTexture();
    static Entity* GetSelectedEnemy();
    static Entity* GetEntityFromGameObject(gce::GameObject* go);	

    static TileMap* GetTileMap();
	static void SetTileMap(TileMap* tileMap);
private:
    //----------Weapon---------
    gce::Geometry* m_thomsonGeo = gce::GeometryFactory::LoadGeometry("res/Obj/thomson.obj");
    gce::Geometry* m_nerfGeo = gce::GeometryFactory::LoadGeometry("res/Obj/nerf.obj");
    gce::Geometry* m_surgarCaneGeo = gce::GeometryFactory::LoadGeometry("res/Obj/canne a sucre.obj");
    //-----------Ennemies---------
    gce::Geometry* m_elfGeo = gce::GeometryFactory::LoadGeometry("res/Obj/lutin.obj");
    gce::Geometry* m_snowManGeo = gce::GeometryFactory::LoadGeometry("res/Obj/bonhomme de neige.obj");
    gce::Geometry* m_robotGeo = gce::GeometryFactory::LoadGeometry("res/Obj/robot.obj");
    gce::Geometry* m_deerGeo = gce::GeometryFactory::LoadGeometry("res/Obj/renne.obj");
    gce::Geometry* m_SantaGeo = gce::GeometryFactory::LoadGeometry("res/Obj/pere noel.obj");

    gce::Vector<Entity*> m_entities;
    gce::Geometry* bulletGeo = gce::GeometryFactory::LoadGeometry("res/Exemple/bottle.obj");
    gce::Texture* m_bottleTexture = new gce::Texture("res/Exemple/TexturesTest.jpg");
    inline static RessourcesManager* m_instance = nullptr;
    Player* m_player = nullptr;
    TileMap* m_tileMap = nullptr;
	Entity* m_enemySelected = nullptr;
	ImportedLevelData* m_importedLevelData = nullptr;
};