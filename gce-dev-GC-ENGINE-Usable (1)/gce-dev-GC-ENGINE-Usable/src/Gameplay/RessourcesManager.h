#pragma once
#include <Engine.h>

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
    
    static gce::Geometry* GetBottle();
    static gce::Texture* GetTexture();
    static Entity* GetEntityFromGameObject(gce::GameObject* go);

private:
    gce::Vector<Entity*> m_entities;
    gce::Geometry* bulletGeo = gce::GeometryFactory::LoadGeometry("res/Exemple/bottle.obj");
    gce::Texture* m_bottleTexture = new gce::Texture("res/Exemple/TexturesTest.jpg");
    inline static RessourcesManager* m_Instance = nullptr;
    Player* m_player = nullptr;
};