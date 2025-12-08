#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"

using namespace gce;

void OnStartShootSnowman(GameObject* me) {

}

void OnUpdateShootSnowman(GameObject* me) {
    std::cout << "shoot";
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    SnowMan* self = dynamic_cast<SnowMan*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 snowmanPos = me->transform.GetWorldPosition();

    Vector3f32 direction = playerPos - snowmanPos;
    direction.Normalize();

    float yaw = atan2f(direction.x, direction.z);
    float pitch = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));

    me->transform.SetWorldRotation(Vector3f32(pitch, yaw, 0.0f));

    if (self->m_ShootCooldown <= 0.0f)
    {
        GameObject* obj = me;
        Scene* scene = const_cast<Scene*>(obj->GetScene());
        GameObject& BulletObject = GameObject::Create(*scene);
        Vector3f32 position = obj->transform.GetWorldPosition();
        Vector3f32 forward = obj->transform.GetWorldForward();

        float spawnOffset = 1.0f;

        Vector3f32 spawnPosition = position + forward * spawnOffset;

        BulletObject.transform.SetWorldPosition(spawnPosition);
        BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());

        MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
        pWeaponRenderer->SetGeometry(RessourcesManager::GetBottle());
        Texture* pWeaponTexture = RessourcesManager::GetTexture();
        pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);
        BulletObject.transform.LocalScale({ 0.25,0.25,0.25 });
        BulletObject.AddComponent<BoxCollider>()->SetActive(true);
        BulletObject.AddComponent<PhysicComponent>();
        BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
        Bullet* bullet = new Bullet(&BulletObject);
        bullet->AddShoot();
        bullet->SetOwner(obj);
        RessourcesManager::AddEntities(bullet);
        self->m_ShootCooldown = 1.0f;
    }
    self->m_ShootCooldown -= GameManager::DeltaTime();
}


void OnEndShootSnowman(GameObject* me) {

}

void OnStartIdleSnowman(GameObject* me) {

}

void OnUpdateIdleSnowman(GameObject* me) {

}


void OnEndIdleSnowman(GameObject* me) {

}