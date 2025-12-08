#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "SnowManState.cpp"

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
    
    SnowMan::SnowMan(GameObject* obj, float spd) : Ennemy(obj, spd)
    {
        MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
        pPlayerRenderer->SetGeometry(SHAPES.CUBE);
        Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
        pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
        obj->AddComponent<BoxCollider>()->SetActive(true);
        obj->AddComponent<PhysicComponent>();
        obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
        obj->SetName("SnowMan");

        StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
        String idle = "Idle";
        String Chase = "Chase";
        String attack = "Attack";

        { // ATTACK STATE
            sm->AddAction(attack, &OnStartShootSnowman, &OnUpdateShootSnowman, &OnEndShootSnowman);
            Vector<StateMachine::Condition> conds;
            conds.PushBack(
                {
                    [](GameObject* me)->bool {
                        Player* p = RessourcesManager::GetPlayer();
                        if (!p) return false;
                        StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                        if (smLocal && smLocal->actualAction == "Attack") return false;
                        Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                        return d.Norm() < 12.0f;
                    }
                }
            );
            sm->AddTransition(conds, attack);
        }
        { // IDLE STATE
            sm->AddAction(idle, &OnStartIdleSnowman, &OnUpdateIdleSnowman, &OnEndIdleSnowman);
            Vector<StateMachine::Condition> conds;
            conds.PushBack(
                {
                    [](GameObject* me)->bool {
                        Player* p = RessourcesManager::GetPlayer();
                        if (!p) return false;
                        StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                        if (smLocal && smLocal->actualAction == "Idle") return false;
                        Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                        return d.Norm() > 12.0f;
                    }
                }
            );
            sm->AddTransition(conds, idle);
        }
        AddLifeScript();
    }
        
void SnowMan::Die()
{
    
}

void SnowMan::Attack()
{
   
}
