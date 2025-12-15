#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include <Engine.h>

using namespace gce;

static void OnStartEmptyBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(me);
    if (!self) return;

	std::cout << "Boss State Started: " << sm->actualAction << std::endl;
}

static void OnEndEmptyBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(me);
    if (!sm) return;

    if (sm->actualAction == "GroundSlam" && self->m_isSlamming)
    {
        self->m_isSlamming = false;
        self->m_hasHitPlayer = false;

        for (GameObject* segment : self->m_slamWaveSegments)
        {
            if (segment)
                segment->Destroy();
        }
        self->m_slamWaveSegments.clear();

        if (self->m_slamWave)
        {
            self->m_slamWave->Destroy();
            self->m_slamWave = nullptr;
        }
    }

    if (sm->actualAction == "Laser" && self->m_isLasering)
    {
        self->m_isLasering = false;

        for (GameObject* segment : self->m_laserBeamSegments)
        {
            if (segment)
                segment->Destroy();
        }
        self->m_laserBeamSegments.clear();
    }
}

static void OnUpdateShootBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;

    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 bossPos = me->transform.GetWorldPosition();

    Vector3f32 dir = playerPos - bossPos;
    dir.Normalize();

    float yaw = atan2f(dir.x, dir.z);
    float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
    me->transform.SetWorldRotation({ pitch, yaw, 0.f });

    if (!self->IsReady("Shoot"))
        return;

    Scene* scene = const_cast<Scene*>(me->GetScene());
    GameObject& bulletObj = GameObject::Create(*scene);

    Vector3f32 spawnPos = bossPos + me->transform.GetWorldForward() * 1.0f;
    bulletObj.transform.SetWorldPosition(spawnPos);
    bulletObj.transform.SetWorldRotation(me->transform.GetWorldRotation());
    bulletObj.transform.WorldScale({ 0.8f,0.8f,0.8f });

    Bullet* bullet = new Bullet(&bulletObj);
    bullet->SetOwner(me);

    self->Use("Shoot");
}

static void OnUpdateIdleBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;

    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 bossPos = me->transform.GetWorldPosition();

    Vector3f32 dir = playerPos - bossPos;
    dir.Normalize();

    float yaw = atan2f(dir.x, dir.z);
    float pitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));
    me->transform.SetWorldRotation({ pitch, yaw, 0.f });

    self->SetCurrentTargetNodePosition();
    self->GeneratePathToPlayer(player->GetGameObject());
    self->FollowPath();
}

static void OnUpdateHeavyMeleeBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;

    if (!self->IsReady("HeavyMelee"))
        return;

    Vector3f32 d =
        player->GetGameObject()->transform.GetWorldPosition() -
        me->transform.GetWorldPosition();

    if (d.Norm() < 5.f)
        player->TakeDamage(3);

    self->Use("HeavyMelee");
}

static void OnUpdateGroundSlamBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;
    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;
    float dt = GameManager::DeltaTime();

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(me);
    if (self->m_isSlamming && sm) {
        sm->actualAction = "GroundSlam";
    }

    if (!self->m_isSlamming)
    {
        if (!self->IsReady("GroundSlam"))
            return;

        self->m_isSlamming = true;
        self->m_slamTimer = 0.f;
        self->m_slamRadius = 1.0f;
        self->m_hasHitPlayer = false;
        self->Use("GroundSlam");
        std::cout << "Ground Slam Started!" << std::endl;

        Scene* scene = const_cast<Scene*>(me->GetScene());

        self->m_slamWave = &GameObject::Create(*scene);
        self->m_slamWave->SetName("ShockwaveRing");
        self->m_slamWave->transform.SetWorldPosition({
            me->transform.GetWorldPosition().x,
            -9.3f,
            me->transform.GetWorldPosition().z
            });

        int numSegments = 128;
        for (int i = 0; i < numSegments; i++)
        {
            float angle = (i / (float)numSegments) * 2.0f * 3.14159f;

            GameObject* segment = &GameObject::Create(*scene);
            segment->SetName("ShockwaveSegment");

            float x = me->transform.GetWorldPosition().x + cosf(angle) * self->m_slamRadius;
            float z = me->transform.GetWorldPosition().z + sinf(angle) * self->m_slamRadius;

            segment->transform.SetWorldPosition({ x, -9.3f, z });
            segment->transform.WorldScale({ 0.5f, 0.5f, 0.5f });

            MeshRenderer* mr = segment->AddComponent<MeshRenderer>();
            mr->SetGeometry(SHAPES.CUBE);

            self->m_slamWaveSegments.push_back(segment);
        }

        return;
    }

    self->m_slamTimer += dt;

    float t = self->m_slamTimer / self->m_slamDuration;
    t = gce::Min(t, 1.f);

    float previousRadius = self->m_slamRadius;
    self->m_slamRadius = 1.0f + (t * (self->m_slamMaxRadius - 1.0f));

    int numSegments = self->m_slamWaveSegments.size();
    for (int i = 0; i < numSegments; i++)
    {
        float angle = (i / (float)numSegments) * 2.0f * 3.14159f;

        float x = me->transform.GetWorldPosition().x + cosf(angle) * self->m_slamRadius;
        float z = me->transform.GetWorldPosition().z + sinf(angle) * self->m_slamRadius;

        self->m_slamWaveSegments[i]->transform.SetWorldPosition({ x, -9.3f, z });
    }

    if (!self->m_hasHitPlayer)
    {
        Vector3f32 bossPos = me->transform.GetWorldPosition();
        Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
        float dist = (playerPos - bossPos).Norm();

        float hitZoneThickness = 2.0f;
        if (dist >= previousRadius - hitZoneThickness &&
            dist <= self->m_slamRadius + hitZoneThickness)
        {
            player->TakeDamage(4);
            self->m_hasHitPlayer = true;
        }
    }

    if (t >= 1.f)
    {
        self->m_isSlamming = false;
        self->m_hasHitPlayer = false;

        for (GameObject* segment : self->m_slamWaveSegments)
        {
            if (segment)
                segment->Destroy();
        }
        self->m_slamWaveSegments.clear();

        if (self->m_slamWave)
        {
            self->m_slamWave->Destroy();
            self->m_slamWave = nullptr;
        }
    }
}

static void OnUpdateTeleportBoss(GameObject* me) {
	me->transform.SetWorldPosition({ float(rand() % 100 + 1),-9.3f, float(rand() % 100 + 1) });
}

static void OnUpdateLaserBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;

    Player* player = RessourcesManager::GetPlayer();
    if (!player) return;

    float dt = GameManager::DeltaTime();

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(me);
    if (self->m_isLasering && sm) {
        sm->actualAction = "Laser";
    }

    if (!self->m_isLasering)
    {
        if (!self->IsReady("Laser"))
            return;

        self->m_isLasering = true;
        self->m_laserTimer = 0.f;
        self->m_laserDamageTimer = 0.f;
        self->m_laserCurrentYaw = 0.f;
        self->m_laserCurrentPitch = 0.f;
        self->Use("Laser");

        Scene* scene = const_cast<Scene*>(me->GetScene());

  
        float laserLength = 50.0f;
        int numSegments = 25;

        for (int i = 0; i < numSegments; i++)
        {
            GameObject* segment = &GameObject::Create(*scene);
            segment->SetName("LaserSegment");

            MeshRenderer* mr = segment->AddComponent<MeshRenderer>();
            mr->SetGeometry(SHAPES.CUBE);
            segment->transform.WorldScale({ 0.3f, 0.3f, 0.3f });

            self->m_laserBeamSegments.push_back(segment);
        }

        Vector3f32 bossPos = me->transform.GetWorldPosition();
        Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
        Vector3f32 dir = playerPos - bossPos;
        dir.Normalize();
        self->m_laserCurrentYaw = atan2f(dir.x, dir.z);
        self->m_laserCurrentPitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));

        return;
    }
    self->m_laserTimer += dt;
    self->m_laserDamageTimer += dt;

    Vector3f32 bossPos = me->transform.GetWorldPosition();
    Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
    Vector3f32 dir = playerPos - bossPos;
    dir.Normalize();
    float targetYaw = atan2f(dir.x, dir.z);
    float targetPitch = atan2f(-dir.y, sqrtf(dir.x * dir.x + dir.z * dir.z));

    float Speed = 0.03f;
    self->m_laserCurrentYaw += (targetYaw - self->m_laserCurrentYaw) * Speed;
    self->m_laserCurrentPitch += (targetPitch - self->m_laserCurrentPitch) * Speed;

    me->transform.SetWorldRotation({ self->m_laserCurrentPitch, self->m_laserCurrentYaw, 0.f });

    float cosYaw = cosf(self->m_laserCurrentYaw);
    float sinYaw = sinf(self->m_laserCurrentYaw);
    float cosPitch = cosf(self->m_laserCurrentPitch);
    float sinPitch = sinf(self->m_laserCurrentPitch);

    Vector3f32 forward = {
        sinYaw * cosPitch,
        -sinPitch,
        cosYaw * cosPitch
    };
    forward.Normalize();

    float laserLength = 50.0f;
    int numSegments = self->m_laserBeamSegments.size();

    for (int i = 0; i < numSegments; i++)
    {
        float segmentDistance = (i / (float)numSegments) * laserLength;
        Vector3f32 segmentPos = bossPos + forward * segmentDistance;

        self->m_laserBeamSegments[i]->transform.SetWorldPosition(segmentPos);
    }

    Vector3f32 toPlayer = playerPos - bossPos;
    float distToPlayer = toPlayer.Norm();

    if (distToPlayer < laserLength)
    {
        float dotProduct = toPlayer.x * forward.x + toPlayer.y * forward.y + toPlayer.z * forward.z;

        if (dotProduct > 0)
        {
            Vector3f32 closestPoint = bossPos + forward * dotProduct;
            Vector3f32 distVec = playerPos - closestPoint;
            float distFromLaser = distVec.Norm();

            float laserRadius = 1.0f;

            if (distFromLaser < laserRadius)
            {
                if (self->m_laserDamageTimer >= 0.2f)
                {
                    player->TakeDamage(1);
                    self->m_laserDamageTimer = 0.f;
                }
            }
        }
    }

    if (self->m_laserTimer >= self->m_laserDuration)
    {
        self->m_isLasering = false;

        for (GameObject* segment : self->m_laserBeamSegments)
        {
            if (segment)
                segment->Destroy();
        }
        self->m_laserBeamSegments.clear();
    }
}


static void OnStartShieldBoss(GameObject* me) {
    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;
    self->m_hasAlreadyShielded = true;
    self->m_isShielded = true;
}

static void OnUpdateShieldBoss(GameObject* me) {

    Entity* ent = RessourcesManager::GetEntityFromGameObject(me);
    Boss* self = dynamic_cast<Boss*>(ent);
    if (!self) return;
    if (self->m_maxShieldDuration <= 0.0f) {
        self->m_isShielded = false;
    }

	self->m_maxShieldDuration -= GameManager::DeltaTime();
    //teleport to a safe zone above
	//Ennemies Spawn Logic Here 
	// when all enemies are dead desactvate shield
}

