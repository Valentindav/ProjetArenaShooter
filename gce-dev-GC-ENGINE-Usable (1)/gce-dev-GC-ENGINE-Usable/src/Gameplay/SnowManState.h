#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "MenuManager.h"

using namespace gce;

static void OnStartShootSnowman(GameObject* me) {

}

static void OnUpdateShootSnowman(GameObject* me) {
    // Ne pas tirer si l'on est dans un menu
    MenuManager* mm = MenuManager::GetInstance();
    if (mm && mm->GetGameState() != GameState::Playing)
        return;

    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    SnowMan* self = dynamic_cast<SnowMan*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
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

        BulletObject.transform.SetWorldPosition({ spawnPosition.x,spawnPosition.y + 1.f,spawnPosition.z });
        BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());

        Bullet* bullet = new Bullet(&BulletObject,500);
		BulletObject.transform.LocalScale({ 5.f,5.f,5.f });
        bullet->SetOwner(me);
        bullet->SetLifeTime(10);
        self->m_ShootCooldown = 2.0f;
    }
    self->m_ShootCooldown -= GameManager::DeltaTime();
}


static void OnEndShootSnowman(GameObject* me) {

}

static void OnStartIdleSnowman(GameObject* me) {

}

static void OnUpdateIdleSnowman(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    SnowMan* self = dynamic_cast<SnowMan*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 snowmanPos = me->transform.GetWorldPosition();    

    Vector3f32 direction = playerPos - snowmanPos;
    direction.Normalize();

    float yaw = atan2f(direction.x, direction.z);
    float pitch = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));

    me->transform.SetWorldRotation(Vector3f32(pitch, yaw, 0.0f));
    self->SetCurrentTargetNodePosition();
    self->GeneratePathToPlayer(player->GetGameObject());
    self->FollowPath();

	if (self->GetCurrentPath().size() < 2) return;
}


static void OnEndIdleSnowman(GameObject* me) {

}