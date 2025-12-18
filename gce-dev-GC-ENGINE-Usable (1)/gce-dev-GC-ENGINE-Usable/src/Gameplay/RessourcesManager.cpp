#include "RessourcesManager.h"
#include "Entity.h"
#include "Player.h"
#include "Ennemy.h" 
#include "JsonImporter.hpp"
#include "Heal.h"

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

gce::Vector<Entity*> RessourcesManager::GetEntities() // get entities vector
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

gce::Geometry* RessourcesManager::GetBrokenSugarCane()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_brokenSurgarCaneGeo;
}

gce::Geometry* RessourcesManager::GetGift()
{
    if (m_instance == nullptr) return nullptr;
    return m_instance->m_explodingGeo;
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

 Entity* RessourcesManager::GetChoosedEnemy()
 {
     if (m_instance == nullptr || m_instance->m_choosedEnemy == nullptr) return nullptr;
	 return m_instance->m_choosedEnemy;
 }

 void RessourcesManager::SetChoosedEnemy(Entity* enemy)
 {
     if (m_instance == nullptr || enemy == nullptr) Create();
	 m_instance->m_choosedEnemy = enemy;
 }

 void RessourcesManager::AddCookie(Heal* cookie)
 {
     if (m_instance == nullptr || cookie == nullptr) Create();
	 m_instance->m_cookiesVector.PushBack(cookie);
 }

 gce::Vector<Heal*> RessourcesManager::GetCookies()
 {
     if (m_instance == nullptr) return {};
	 return m_instance->m_cookiesVector;
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

void RessourcesManager::AddLevel(ImportedLevelData level)
{
    if (m_instance == nullptr) Create();
    if (m_instance->m_importedLevelData != nullptr)
    {
        delete m_instance->m_importedLevelData;
        m_instance->m_importedLevelData = nullptr;
    }
    m_instance->m_importedLevelData = new ImportedLevelData(level);
}

void RessourcesManager::ClearCurrentLevel()
{
    if (m_instance == nullptr || m_instance->m_importedLevelData == nullptr)
        return;

    if (m_instance->m_importedLevelData->root)
    {
		m_instance->m_importedLevelData->root->Destroy();
    }
}
Vector3f32 RessourcesManager::GetEnemySpawnPosition(float32 minDist)
{
    if (m_instance == nullptr || m_instance->m_importedLevelData == nullptr || m_instance->m_importedLevelData->spawnZones.empty() || m_instance->m_player == nullptr || m_instance->m_player->GetGameObject() == nullptr)
    {
        return { 0.f,0.f,0.f };
    }
    size_t randomIndex = static_cast<size_t>(std::rand()) % m_instance->m_importedLevelData->spawnZones.size();
    gce::Vector3f32 spawnZone = m_instance->m_importedLevelData->spawnZones[randomIndex];
    while ((m_instance->m_player->GetGameObject()->transform.GetWorldPosition() - spawnZone).Norm() < minDist)
    {
        randomIndex = static_cast<size_t>(std::rand()) % m_instance->m_importedLevelData->spawnZones.size();
        spawnZone = m_instance->m_importedLevelData->spawnZones[randomIndex];
	}
	return spawnZone;
}

void RessourcesManager::AddEnnemy(Ennemy* ennemy)
{
    if (m_instance == nullptr) Create();
    m_instance->m_ennemies.PushBack(ennemy);
}

gce::Vector<Ennemy*> RessourcesManager::GetEnnemies()
{
    if (m_instance == nullptr) return {};
    return m_instance->m_ennemies;
}

void RessourcesManager::SpawnEnnemies(int indice, float32 minDistance)
{
    if (m_instance == nullptr || m_instance->m_ennemies.Size() < indice) return;
	m_instance->m_ennemies[indice]->GetGameObject()->transform.SetWorldPosition(GetEnemySpawnPosition(minDistance));
}

void RessourcesManager::SetupLevelData(ImportedLevelData levelData)
{
    if (m_instance == nullptr) Create();
    if (m_instance->m_importedLevelData != nullptr)
    {
        delete m_instance->m_importedLevelData;
        m_instance->m_importedLevelData = nullptr;
    }
    m_instance->m_importedLevelData = new ImportedLevelData(levelData);
}

void RessourcesManager::AddMaterial(const MaterialData& mat)
{
    if (m_instance == nullptr) Create();
    m_instance->m_materials.push_back(mat);
}

MaterialData* RessourcesManager::GetMaterial(int index)
{
    if (m_instance == nullptr) return nullptr;
    if (index < 0 || index >= static_cast<int>(m_instance->m_materials.size())) return nullptr;
    return &m_instance->m_materials[index];
}

void RessourcesManager::AssignMaterialToRenderer(gce::MeshRenderer* mr, int index)
{
    if (m_instance == nullptr || mr == nullptr) return;
    MaterialData* mat = GetMaterial(index);
    if (mat == nullptr) return;
    if (mat->albedo)
		mr->SetAlbedoTexture(mat->albedo);
    if (mat->normal)
		mr->SetNormalTexture(mat->normal);
    if (mat->metallic)
		mr->SetMetalnessTexture(mat->metallic);
    if (mat->roughness)
		mr->SetRoughnessTexture(mat->roughness);
    if (mat->displacement)
		mr->SetDisplacementTexture(mat->displacement);
}

void RessourcesManager::CreateMaterials(String albedoPath, String normalPath, String metallicPath, String roughnessPath, String displacementPath)
{
    if (m_instance == nullptr) Create();
    MaterialData mat;
    if (!albedoPath.empty())
        mat.albedo = new gce::Texture(albedoPath);
    if (!normalPath.empty())
        mat.normal = new gce::Texture(normalPath);
    if (!metallicPath.empty())
        mat.metallic = new gce::Texture(metallicPath);
    if (!roughnessPath.empty())
        mat.roughness = new gce::Texture(roughnessPath);
    if (!displacementPath.empty())
        mat.displacement = new gce::Texture(displacementPath);
    m_instance->m_materials.push_back(mat);
}