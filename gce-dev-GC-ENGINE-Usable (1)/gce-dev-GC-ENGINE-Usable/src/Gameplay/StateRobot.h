#include "Robot.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"

using namespace gce;

static void OnStartShootRobot(GameObject* me) {

}

static void OnUpdateShootRobot(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Robot* self = dynamic_cast<Robot*>(ent);
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
    /*if (self->m_WaitCooldown > 0.0f && self->m_WaitCooldown < 2.0f)
    {


    }
    else*/	if (self->m_WaitCooldown <= 0.0f)
    {
        GameObject* obj = me;
        Scene* scene = const_cast<Scene*>(obj->GetScene());
        GameObject& BulletObject = GameObject::Create(*scene);
        Vector3f32 position = obj->transform.GetWorldPosition();
        Vector3f32 forward = obj->transform.GetWorldForward();

        BulletObject.transform.WorldScale({ 1,1,100 });
        float spawnOffset = BulletObject.transform.GetWorldScale().z/100;

        Vector3f32 spawnPosition = position + forward * spawnOffset;

        BulletObject.transform.SetWorldPosition(spawnPosition);
        BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());

        MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
        pWeaponRenderer->SetGeometry(SHAPES.CUBE);
        Texture* pWeaponTexture = RessourcesManager::GetTexture();
        pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);
        BulletObject.AddComponent<BoxCollider>()->SetActive(true);
        BulletObject.AddComponent<PhysicComponent>();
        BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
        Bullet* bullet = new Bullet(&BulletObject);
        bullet->AddShoot();
        bullet->SetOwner(me);
        bullet->m_speed = 0.0f;
        bullet->GetGameObject()->GetComponent<PhysicComponent>()->SetIsTrigger(true);
        self->m_WaitCooldown = 2.0f;
    }
    self->m_WaitCooldown -= GameManager::DeltaTime();
}


static void OnEndShootRobot(GameObject* me) {

}

static void OnStartIdleRobot(GameObject* me) {

}

static void OnUpdateIdleRobot(GameObject* me) {

}


static void OnEndIdleRobot(GameObject* me) {

}