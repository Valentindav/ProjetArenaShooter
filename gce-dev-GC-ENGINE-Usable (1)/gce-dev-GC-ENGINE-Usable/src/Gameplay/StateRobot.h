#include "Robot.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "MenuManager.h"

using namespace gce;

static void OnStartShootRobot(GameObject* me) {

}

static void OnUpdateShootRobot(GameObject* me) { // update for robot shooting state with 3 state attack patern
    MenuManager* mm = MenuManager::GetInstance();
    if (mm && mm->GetGameState() != GameState::Playing)
        return;

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

    if (self->m_laserPhase == 0) {
    
        me->transform.SetWorldRotation(targetEuler);
        Vector3f32 d = playerPos - robotPos;
            self->m_lockedRotation = targetEuler;
            self->m_laserPhase = 1;
            self->m_laserTimer = 2.0f; 
        return;
    }

    if (self->m_laserPhase == 1) {
        me->transform.SetWorldRotation(self->m_lockedRotation);
        if (!self->m_laser) {
            Scene* scene = (Scene*)me->GetScene();
            GameObject& objLaser = GameObject::Create(*scene);

            Vector3f32 forward = me->transform.GetWorldForward();
            Vector3f32 spawnPos = robotPos + forward * 2.5f;

            objLaser.transform.WorldScale({ 1,1,100 });
            objLaser.transform.SetWorldPosition(spawnPos);
            objLaser.transform.SetWorldRotation(self->m_lockedRotation);

            self->m_laser = new Bullet(&objLaser);
            self->m_laser->SetOwner(me);
            self->m_laser->m_speed = 0.0f;
            self->m_laser->SetDamage(0);
            self->m_laser->SetLifeTime(1000.f);
        }

        self->m_laserTimer -= GameManager::DeltaTime();
        if (self->m_laserTimer <= 0.0f) {
            self->m_laserPhase = 2;
            self->m_laserTimer = 1.5f;
        }
        return;

    }
    if (self->m_laserPhase == 2) {
        me->transform.SetWorldRotation(self->m_lockedRotation);
        if (self->m_laser) {
            self->m_laser->SetDamage(5);
            self->m_laser->SetNotCollide(true);
        }

        self->m_laserTimer -= GameManager::DeltaTime();
        if (self->m_laserTimer <= 0.0f) {
            if (self->m_laser) {
                delete self->m_laser;
                self->m_laser = nullptr;
            }
            self->m_laserPhase = 0; 
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