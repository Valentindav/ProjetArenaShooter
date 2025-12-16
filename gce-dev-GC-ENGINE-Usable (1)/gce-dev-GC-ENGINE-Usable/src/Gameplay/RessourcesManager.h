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

    /*static void RegisterSpawnZone(gce::GameObject* zone);*/
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


    static bool IsZoneSafeForPlayer(gce::BoxCollider* zoneCol);
    static bool CheckAABBOverlap(const gce::Box& b1, const gce::Box& b2);

public:

    template <typename EnemyType>
    static EnemyType* SpawnRandomEnemy()
    {
        if (m_instance == nullptr || m_instance->m_importedLevelData->spawnZones.empty()) {
            std::cout << "[Spawn] Pas de zones ou Manager non init !\n";
            return nullptr;
        }

        // 1. Mélanger les zones pour essayer dans un ordre aléatoire
        // (Copie locale pour ne pas modifier l'ordre original)
        std::vector<gce::GameObject*> zonesToCheck = m_instance->m_importedLevelData->spawnZones;
        // Mélange simple
        for (size_t i = 0; i < zonesToCheck.size(); ++i) {
            std::swap(zonesToCheck[i], zonesToCheck[rand() % zonesToCheck.size()]);
        }

        gce::BoxCollider* safeZone = nullptr;

        // 2. Trouver une zone sûre (sans le joueur)
        for (auto* zoneObj : zonesToCheck)
        {
            auto* col = zoneObj->GetComponent<gce::BoxCollider>();
            if (!col) continue;

            if (IsZoneSafeForPlayer(col)) {
                safeZone = col;
                break; // On a trouvé !
            }
        }

        if (!safeZone) {
            std::cout << "[Spawn] Aucune zone safe (le joueur bloque tout ?)\n";
            return nullptr;
        }

        // 3. Calculer position aléatoire DANS la Box
        gce::Box box = safeZone->GetWorldBox();
        float rX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float rZ = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

        float x = box.min.x + rX * (box.max.x - box.min.x);
        float z = box.min.z + rZ * (box.max.z - box.min.z);
        float y = box.min.y; // Au sol

        // 4. Créer l'ennemi
        // On récupère la scène active via le joueur ou le singleton
        gce::Scene* scene = gce::GameManager::GetScenes()[0];
        gce::GameObject& enemyObj = gce::GameObject::Create(*scene);

        enemyObj.transform.SetWorldPosition({ x, y, z });

        // Création de l'instance de l'ennemi (Robot, SnowMan, etc.)
        // Note: Le constructeur doit accepter (GameObject*)
        EnemyType* newEnemy = new EnemyType(&enemyObj);

        // Ajout automatique à la liste des entités
        AddEntities(newEnemy);

        std::cout << "[Spawn] Ennemi cree en " << x << ", " << z << "\n";
        return newEnemy;
    }
};