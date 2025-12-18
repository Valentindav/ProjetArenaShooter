#include "Robot.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"

using namespace gce;

static void OnStartShootRobot(GameObject* me) {

}

static void OnUpdateShootRobot(GameObject* me) { // update for robot shooting state with 3 state attack patern
    Robot* self = dynamic_cast<Robot*>(RessourcesManager::GetEntityFromGameObject(me));
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 robotPos = me->transform.GetWorldPosition();
    Vector3f32 dir = playerPos - robotPos;
    dir.Normalize();

    float yaw = atan2f(dir.x, dir.z);
    float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
    Vector3f32 targetEuler = Vector3f32(pitch, yaw, 0.0f);

    if (self->laserPhase == 0) {
    
        me->transform.SetWorldRotation(targetEuler);
        Vector3f32 d = playerPos - robotPos;
            self->lockedRotation = targetEuler;
            self->laserPhase = 1;
            self->laserTimer = 5.0f; 
        return;
    }

    if (self->laserPhase == 1) {
        me->transform.SetWorldRotation(self->lockedRotation);

        if (!self->laser) {
            Scene* scene = (Scene*)me->GetScene();
            GameObject& objLaser = GameObject::Create(*scene);

            Vector3f32 forward = me->transform.GetWorldForward();
            Vector3f32 spawnPos = robotPos + forward * 2.5f;

            objLaser.transform.WorldScale({ 1,1,100 });
            objLaser.transform.SetWorldPosition(spawnPos);
            objLaser.transform.SetWorldRotation(self->lockedRotation);

            self->laser = new Bullet(&objLaser);
            self->laser->SetOwner(me);
            self->laser->m_speed = 0.0f;
           // self->laser->SetDamage(0);
            //self->laser->SetLifeTime(5.f);
        }

        self->laserTimer -= GameManager::DeltaTime();
        if (self->laserTimer <= 0.0f) {
            self->laserPhase = 2;
            self->laserTimer = 1.5f;
        }
        return;
    }
    if (self->laserPhase == 2) {
        me->transform.SetWorldRotation(self->lockedRotation);

        if (self->laser) {
           // self->laser->SetDamage(5);
        }

        self->laserTimer -= GameManager::DeltaTime();
        if (self->laserTimer <= 0.0f) {
            if (self->laser) {
                delete self->laser;
                self->laser = nullptr;
            }
            self->laserPhase = 0; 
        }
        return;
    }
}


static void OnEndShootRobot(GameObject* me) {

}

static void OnStartIdleRobot(GameObject* me) {

}

static void OnUpdateIdleRobot(GameObject* me) {

}


static void OnEndIdleRobot(GameObject* me) {

}