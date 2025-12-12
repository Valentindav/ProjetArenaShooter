#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"

using namespace gce;

static void OnStartEmptyBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(me);
    if (!self) return;

	std::cout << "Boss State Started: " << sm->actualAction << std::endl;
}

static void OnEndEmptyBoss(GameObject* me) {

}

static void OnUpdateShootBoss(GameObject* me) {
   /* Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 BossPos = me->transform.GetWorldPosition();

    Vector3f32 direction = playerPos - BossPos;
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
        BulletObject.transform.WorldScale({ 0.25,0.25,0.25 });

        Bullet* bullet = new Bullet(&BulletObject);
        bullet->SetOwner(me);
        self->m_ShootCooldown = 2.0f;
    }
    self->m_ShootCooldown -= GameManager::DeltaTime();*/
}

static void OnUpdateIdleBoss(GameObject* me) {
  /*  Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 BossPos = me->transform.GetWorldPosition();

    Vector3f32 direction = playerPos - BossPos;
    direction.Normalize();

    float yaw = atan2f(direction.x, direction.z);
    float pitch = atan2f(-direction.y, sqrtf(direction.x * direction.x + direction.z * direction.z));

    me->transform.SetWorldRotation(Vector3f32(pitch, yaw, 0.0f));
    self->SetCurrentTargetNodePosition();
    self->GeneratePathToPlayer(player->GetGameObject());
    self->FollowPath();*/
}

static void OnUpdateHeavyMeleeBoss(GameObject* me) {

}

static void OnUpdateGroundSlamBoss(GameObject* me) {

}

static void OnUpdateTeleportBoss(GameObject* me) {

}

static void OnUpdateLaserBoss(GameObject* me) {

}

static void OnUpdateShieldBoss(GameObject* me) {
/*	Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
	Boss* self = dynamic_cast<Boss*>(ent);
	if (!self) return;
	self->m_hasAlreadyShielded = true;
	self->m_isShielded = true;
    //teleport to a safe zone above
	//Ennemies Spawn Logic Here 
	// when all enemies are dead desactvate shield*/
}

