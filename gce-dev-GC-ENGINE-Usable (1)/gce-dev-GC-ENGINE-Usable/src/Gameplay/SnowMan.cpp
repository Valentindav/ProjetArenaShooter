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
    Node<Tile>* node = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    if (node)
    {
        m_currentNodeIndex.x = static_cast<float>(node->data->gridX);
        m_currentNodeIndex.y = static_cast<float>(node->data->gridY);
    }
}

void SnowMan::GeneratePathToPlayer(GameObject* player) 
{
    if (!m_tileMap) return;

    Node<Tile>* startNode = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    Node<Tile>* targetNode = m_tileMap->GetNodeFromWorldPosition(player->transform.GetWorldPosition());

    if (startNode && targetNode && startNode != targetNode)
    {
        m_currentPath = m_tileMap->GeneratePath(startNode, targetNode);
    }
}

void SnowMan::FollowPath()
{
	if (m_currentPath.size() < 2) return;
    
    Vector3f32 nextPos = {
        m_currentPath[1]->data->worldPosition.x,
        m_gameObject->transform.GetWorldPosition().y,
        m_currentPath[1]->data->worldPosition.y
    };

    Vector3f32 currentPos = m_gameObject->transform.GetWorldPosition();
    Vector3f32 direction = nextPos - currentPos;

    if (direction.SquareNorm() > 0.1f)
    {
        direction.SelfNormalize();
        float speed = 5.0f;
        m_gameObject->transform.SetWorldPosition(currentPos + direction * speed * GameManager::DeltaTime());
    }
}

void SnowMan::AddScript()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<AttackScript>();
}