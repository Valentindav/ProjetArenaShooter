#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "MenuManager.h"
using namespace gce;

static void OnStartShootDeer(GameObject* me) {

}

static void OnUpdateShootDeer(GameObject* me) {
    MenuManager* mm = MenuManager::GetInstance();
    if (mm && mm->GetGameState() != GameState::Playing)
        return;
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Deer* self = dynamic_cast<Deer*>(ent);
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
        Vector3f32 d = player->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
        if (d.Norm() < 2.0f) {
            player->TakeDamage(1);
        }
        self->m_ShootCooldown = 4.0f;
    }
    self->m_ShootCooldown -= GameManager::DeltaTime();
}


static void OnEndShootDeer(GameObject* me) {

}

static void OnStartIdleDeer(GameObject* me) {

}

static void OnUpdateIdleDeer(GameObject* me) {
    MenuManager* mm = MenuManager::GetInstance();
    if (mm && mm->GetGameState() != GameState::Playing)
        return;
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Deer* self = dynamic_cast<Deer*>(ent);
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


static void OnEndIdleDeer(GameObject* me) {

}