#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Player.h"
using namespace gce;

DECLARE_SCRIPT( AttackScript, ScriptFlag::Start | ScriptFlag::Update)
private:
    Geometry* bulletGeo = GeometryFactory::LoadGeometry("res/Exemple/SUZANNE.obj");
    Texture* bulletTex = new Texture("res/Exemple/TexturesTest.jpg");
    float ShootCooldown = 0.75f;

public:
    void Start()
    {
        ShootCooldown = 0.75f;
    }

    void Update()
    {
        Player* player = RessourcesManager::GetPlayer();
        Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
        Vector3f32 snowmanPos = m_pOwner->transform.GetWorldPosition();

        SnowMan* ownerEntity = nullptr;
        for (Entity* p : RessourcesManager::getEntities())
        {
            if (m_pOwner == p->GetGameObject())
            {
                ownerEntity = dynamic_cast<SnowMan*>(p);
                break;
            }
        }
		ownerEntity->SetCurrentTargetNodePosition();
		ownerEntity->GeneratePathToPlayer(player->GetGameObject());
        ownerEntity->FollowPath();

        Vector3f32 direction = playerPos - snowmanPos;
        direction.Normalize();

        float yaw = atan2f(direction.x, direction.z);
        float pitch = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));

        m_pOwner->transform.SetWorldRotation(Vector3f32(pitch, yaw, 0.0f));
        if (ShootCooldown <= 0.0f)
        {
            GameObject* obj = m_pOwner;
            Scene* scene = const_cast<Scene*>(obj->GetScene());
            GameObject& BulletObject = GameObject::Create(*scene);
            Vector3f32 position = obj->transform.GetWorldPosition();
            Vector3f32 forward = obj->transform.GetWorldForward();

            float spawnOffset = 1.0f;

            Vector3f32 spawnPosition = position + forward * spawnOffset;

            BulletObject.transform.SetWorldPosition(spawnPosition);
            BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());

            MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
            pWeaponRenderer->SetGeometry(bulletGeo);
            Texture* pWeaponTexture = bulletTex;
            pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);
            BulletObject.transform.LocalScale({ 0.25,0.25,0.25 });
            BulletObject.AddComponent<BoxCollider>()->SetActive(true);
            BulletObject.AddComponent<PhysicComponent>();
            BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
            Bullet* bullet = new Bullet(&BulletObject);
            bullet->AddShoot();
            bullet->SetOwner(obj);
            RessourcesManager::AddEntities(bullet);
			ShootCooldown = 1.0f;
		} 
        ShootCooldown -= GameManager::DeltaTime();    
    }

END_SCRIPT

SnowMan::SnowMan(GameObject* obj, TileMap* tileMap, float spd) : Ennemy(obj, spd), m_tileMap(tileMap)
    {
        MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
        pPlayerRenderer->SetGeometry(SHAPES.CUBE);
        Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
        pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
        obj->AddComponent<BoxCollider>()->SetActive(true);
        obj->AddComponent<PhysicComponent>();
        obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);

        obj->SetName("SnowMan");

        AddScript();
        AddLifeScript();
    }

void SnowMan::Die()
{
    
}

void SnowMan::Attack()
{
   
}

void SnowMan::SetCurrentTargetNodePosition()
{
	m_currentNodeIndex.x = (m_gameObject->transform.GetWorldPosition().x - m_tileMap->GetOrigin().x) / m_tileMap->GetCellSize();
	m_currentNodeIndex.y = (m_gameObject->transform.GetWorldPosition().z - m_tileMap->GetOrigin().y) / m_tileMap->GetCellSize();
}

void SnowMan::GeneratePathToPlayer(GameObject* player) 
{
	auto grid = m_tileMap->GetNodeVector();
	if (grid.empty()) return;

    int width = static_cast<int>(grid.size());
    int length = static_cast<int>(grid[0].size());
    float cellSize = m_tileMap->GetCellSize();
    Vector2f32 origin = m_tileMap->GetOrigin();

    // Calcul des indices du joueur (Cible)
    Vector3f32 playerPos = player->transform.GetWorldPosition();
    int playerGridX = static_cast<int>((playerPos.x - origin.x) / cellSize);
    int playerGridY = static_cast<int>((playerPos.z - origin.y) / cellSize);

    // Indices du SnowMan (Départ)
    int startX = static_cast<int>(m_currentNodeIndex.x);
    int startY = static_cast<int>(m_currentNodeIndex.y);

    // Vérification des limites (Bounds Check) pour éviter le crash "out of range"
    bool isStartValid = (startX >= 0 && startX < width && startY >= 0 && startY < length);
    bool isTargetValid = (playerGridX >= 0 && playerGridX < width && playerGridY >= 0 && playerGridY < length);
	std::cout << isStartValid << " " << isTargetValid << std::endl;
	std::cout << playerGridX << " " << playerGridY << std::endl;
    if (isStartValid && isTargetValid)
    {
        m_currentPath = m_tileMap->GeneratePath(
            grid[startX][startY],
            grid[playerGridX][playerGridY]
        );
    }
}

void SnowMan::FollowPath()
{
	if (m_currentPath.size() < 2) return;
    m_gameObject->transform.SetWorldPosition({
        (m_currentPath[1]->data->worldPosition.x)*GameManager::DeltaTime(),
        m_gameObject->transform.GetWorldPosition().y,
        (m_currentPath[1]->data->worldPosition.y)* GameManager::DeltaTime()
		});
}

void SnowMan::AddScript()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<AttackScript>();
}