#include "RayCast.h"
#include "Maths.h"
#include "Player.h"

RayCast::RayCast(GameObject* obj, float32 offset) : m_rayCastObject(obj), m_offsetFromPlayer(offset)
{  
    //MeshRenderer* pRayCastRenderer = obj->AddComponent<MeshRenderer>();
    //pRayCastRenderer->SetGeometry(SHAPES.CUBE);
    obj->transform.LocalScale({ 0.1f,0.1f,m_distanceToEnemy });
    obj->transform.LocalTranslate({ 0.f, 0.f, m_distanceToEnemy / 2.f + m_offsetFromPlayer + 1.f });
    BoxCollider* rayCastCollider = obj->AddComponent<BoxCollider>();
	rayCastCollider->SetActive(true);
    rayCastCollider->isTrigger = true;
	obj->SetName("RayCast");
	AddSelectEnemy();
}

DECLARE_SCRIPT(SelectEnemy, ScriptFlag::CollisionStay | ScriptFlag::CollisionExit)

    void CollisionStay(GameObject* other)
    {
        if (m_pOwner && m_pOwner->IsActive())
        {         
			if (!(other->GetName() == "SnowMan" || other->GetName() == "robot")) return;
            Entity* selectedEnemy = RessourcesManager::GetSelectedEnemy();
            if (!selectedEnemy || selectedEnemy == nullptr)
            {
                Entity* entity = RessourcesManager::GetEntityFromGameObject(other);
                if (entity)
                {
                    RessourcesManager::SetEnemySelected(entity);
                }
			}
            else
            {
                Player* player = RessourcesManager::GetPlayer();
                if (!player) return;
                Vector3f32 playerPos = player->GetGameObject()->transform.GetWorldPosition();
                Vector3f32 enemyPos = other->transform.GetWorldPosition();
                float32 dist = (enemyPos - playerPos).Norm();
                if (!selectedEnemy|| selectedEnemy->GetGameObject()||!selectedEnemy->GetGameObject() || selectedEnemy->GetGameObject() == nullptr) return;
                if (dist < (selectedEnemy->GetGameObject()->transform.GetWorldPosition() - playerPos).Norm())
                {
                    Entity* entity = RessourcesManager::GetEntityFromGameObject(other);
                    if (entity)
                    {
                        RessourcesManager::SetEnemySelected(entity);
                    }
                }
            }			
        }
    }

    void CollisionExit(GameObject* other)
    {
		if(!m_pOwner || !m_pOwner->IsActive()) return;
		if (!other) return;
		if (!RessourcesManager::GetSelectedEnemy()) return;
        if (other == RessourcesManager::GetSelectedEnemy()->GetGameObject())
        {
            RessourcesManager::SetEnemySelected(nullptr);
        }
    }

END_SCRIPT

void RayCast::AddSelectEnemy()
    {
        GameObject* obj = GetRayCastObject();
        obj->AddScript<SelectEnemy>();
    }