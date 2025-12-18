#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"

using namespace gce;

static void OnStartShootElf(GameObject* me) {

}

static void OnUpdateShootElf(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Elf* self = dynamic_cast<Elf*>(ent);
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

    if (self->m_ShootCooldown <= 0.0f )
    {
        me->transform.WorldTranslate(me->transform.GetLocalUp() * 50 * GameManager::DeltaTime());
        me->transform.WorldTranslate(me->transform.GetLocalForward() * 200 * GameManager::DeltaTime());
        Vector3f32 d = player->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
        if (d.Norm() < 6.0f) {
            player->TakeDamage(2);
        }
        self->m_ShootCooldown = 4.0f;
    }
    self->m_ShootCooldown -= GameManager::DeltaTime();
}


static void OnEndShootElf(GameObject* me) {

}

static void OnStartIdleElf(GameObject* me) {

}

static void OnUpdateIdleElf(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Elf* self = dynamic_cast<Elf*>(ent);
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
}


static void OnEndIdleElf(GameObject* me) {

}