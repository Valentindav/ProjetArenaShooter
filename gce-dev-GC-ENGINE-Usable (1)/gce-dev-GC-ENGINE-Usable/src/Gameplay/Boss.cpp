#include "Boss.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "BossState.h"
#include <Audio.h>

using namespace gce;

DECLARE_SCRIPT(Time, ScriptFlag::Update)
public : 
    void Update()
    {
		Entity* ent = RessourcesManager::GetEntityFromGameObject(m_pOwner);
		Boss* boss = dynamic_cast<Boss*>(ent);
        boss->m_time += GameManager::DeltaTime();
    }
END_SCRIPT

Boss::Boss(GameObject* obj, TileMap* tileMap, float spd) : Ennemy(obj, spd), m_tileMap(tileMap)
{
    GameObject* boxColliderObj = &GameObject::Create(*const_cast<Scene*>(GetGameObject()->GetScene()));
    MeshRenderer* pBoxColliderRenderer = boxColliderObj->AddComponent<MeshRenderer>();
    pBoxColliderRenderer->SetGeometry(RessourcesManager::GetSanta());
    boxColliderObj->transform.SetWorldPosition({ obj->transform.GetWorldPosition().x,obj->transform.GetWorldPosition().y-2.f,obj->transform.GetWorldPosition().z });

    obj->transform.SetLocalScale({ 2.f, 4.5f,2.f });
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Boss");
    //MeshRenderer* pBoxColliderRnderer = obj->AddComponent<MeshRenderer>();
    //pBoxColliderRnderer->SetGeometry(SHAPES.CUBE);
    obj->AddChild(*boxColliderObj);
    obj->transform.SetLocalScale({ 1.0f, 1.5f, 1.0f });

	m_life = 10;
    m_baseLife = m_life;

    m_cooldown["HeavyMelee"] = 3.f;
    m_cooldown["GroundSlam"] = 10.f;
    m_cooldown["Shoot"] = 2.f;
    m_cooldown["Teleport"] = 6.f;
    m_cooldown["Laser"] = 7.f;
    m_cooldown["Shield"] = 9999.f;

    AudioUse::LoadSound("GroundSlam", m_groundSlam.c_str());
    AudioUse::LoadSound("Laser", m_laser.c_str());
    AudioUse::LoadSound("Shoot", m_shoot.c_str());
    AudioUse::LoadSound("HeavyMelee", m_heavyMelee.c_str());

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
    String idle = "Idle";
    String HeavyMelee = "HeavyMelee";
    String GroundSlam = "GroundSlam";
    String Shoot = "Shoot";
    String Teleport = "Teleport";
    String Laser = "Laser";
    String Shield = "Shield";
    
    { // IDLE STATE
        sm->AddAction(idle, &OnStartEmptyBoss, &OnUpdateIdleBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                if (!boss || boss->m_isShielded || boss->m_isSlamming) return false;

                Player* p = RessourcesManager::GetPlayer();
                if (!p) return false;

                StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                if (!smLocal || smLocal->actualAction == "Idle") return false;
                if(boss->m_isShielded) return false;
                Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                return d.Norm() > 20.0f;
                }
            }
        );
        sm->AddTransition(conds, idle);
    }

    { // HeavyMelee STATE
        sm->AddAction(HeavyMelee, &OnStartEmptyBoss, &OnUpdateHeavyMeleeBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                if (!boss || boss->m_isShielded || boss->m_isSlamming) return false;

                Player* p = RessourcesManager::GetPlayer();
                if (!p) return false;

                StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                if (!smLocal || smLocal->actualAction == "HeavyMelee") return false;

                if (!boss->IsReady("HeavyMelee")) return false;
                if (boss->m_isShielded) return false;
                Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                return d.Norm() < 3.0f;
                }
            }
        );
        sm->AddTransition(conds, HeavyMelee);
    }

    { // GroundSlam STATE
        sm->AddAction(GroundSlam, &OnStartEmptyBoss, &OnUpdateGroundSlamBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                    if (!boss || boss->m_isShielded) return false;

                    if (boss->m_isSlamming) return false;

                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;

                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (!smLocal || smLocal->actualAction == "GroundSlam") return false;

                    if (!boss->IsReady("GroundSlam")) return false;
                    if (boss->m_isShielded) return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() < 8.0f && d.Norm() > 3.0f;
                }
            }
        );
        sm->AddTransition(conds, GroundSlam);
    }

    { // Shoot STATE
        sm->AddAction(Shoot, &OnStartEmptyBoss, &OnUpdateShootBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                if (!boss || boss->m_isShielded || boss->m_isSlamming) return false;

                Player* p = RessourcesManager::GetPlayer();
                if (!p) return false;

                StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                if (!smLocal || smLocal->actualAction == "Shoot") return false;
                if (boss->m_isShielded) return false;
                Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                return d.Norm() < 15.0f && d.Norm() > 8.0f;
                }
            }
        );
        sm->AddTransition(conds, Shoot);
    }

    { // Teleport STATE (DISTANCE AUGMENTÉE)
        sm->AddAction(Teleport, &OnStartEmptyBoss, &OnUpdateTeleportBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                    if (!boss || boss->m_isSlamming) return false;

                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;

                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (!smLocal || smLocal->actualAction == "Teleport") return false;
                    if (boss->m_isShielded) return false;
                    if (!boss->IsReady("Teleport")) return false;
                    if (boss->IsReady("HeavyMelee")) return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() < 5.0f; 
                }
            }
        );
        sm->AddTransition(conds, Teleport);
    }

    { // Laser STATE
        sm->AddAction(Laser, &OnStartEmptyBoss, &OnUpdateLaserBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                if (!boss || boss->m_isShielded || boss->m_isSlamming) return false;

                Player* p = RessourcesManager::GetPlayer();
                if (!p) return false;

                StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                if (!smLocal || smLocal->actualAction == "Laser") return false;

                if (!boss->IsReady("Laser")) return false;
                if (boss->m_isShielded) return false;
                Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                return d.Norm() > 15.0f && d.Norm() < 20.0f ;
                }
            }
        );
        sm->AddTransition(conds, Laser);
    }
    
    { // Shield STATE
        sm->AddAction(Shield, &OnStartShieldBoss, &OnUpdateShieldBoss, &OnEndEmptyBoss);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                Boss* boss = dynamic_cast<Boss*>(RessourcesManager::GetEntityFromGameObject(me));
                if (!boss || boss->m_isShielded || boss->m_isSlamming) return false;

                Player* p = RessourcesManager::GetPlayer();
                if (!p) return false;

                StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                if (!smLocal || smLocal->actualAction == "Shield") return false;

                if (boss->m_life <= (boss->m_baseLife/2) && !boss->m_hasAlreadyShielded && boss->IsReady("Shield"))
                {
                    return true;
                }
                return false;
            }
            }
        );
        sm->AddTransition(conds, Shield);
    }
        sm->Transit(idle);
}

bool Boss::IsReady(const String& atk)
{
    if (m_lastUse.find(atk) == m_lastUse.end())
        return true;

    return (m_time - m_lastUse[atk]) >= m_cooldown[atk];
}

void Boss::Use(const String& atk)
{
    m_lastUse[atk] = m_time;
}

Boss::~Boss()
{
    OnEndEmptyBoss(GetGameObject());
}

void Boss::Die()
{

}

void Boss::Attack()
{

}

void Boss::SetCurrentTargetNodePosition()
{
    Node<Tile>* node = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    if (node)
    {
        m_currentNodeIndex.x = static_cast<float>(node->data->gridX);
        m_currentNodeIndex.y = static_cast<float>(node->data->gridY);
    }
}

void Boss::GeneratePathToPlayer(GameObject* player)
{
    if (!m_tileMap) return;

    Node<Tile>* startNode = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    Node<Tile>* targetNode = m_tileMap->GetNodeFromWorldPosition(player->transform.GetWorldPosition());

    if (startNode && targetNode && startNode != targetNode)
    {
        m_currentPath = m_tileMap->GeneratePath(startNode, targetNode);
    }
}

void Boss::FollowPath()
{
    if (m_currentPath.size() < 2) return;

    Vector3f32 nextPos = {
        m_currentPath[1]->data->worldPosition.x,
        m_gameObject->transform.GetWorldPosition().y,
        m_currentPath[1]->data->worldPosition.y
    };

    Vector3f32 currentPos = m_gameObject->transform.GetWorldPosition();
    Vector3f32 direction = nextPos - currentPos;

    if (direction.SquareNorm() > 0.1f)
    {
        direction.SelfNormalize();
        float speed = 5.0f;
        m_gameObject->transform.SetWorldPosition(currentPos + direction * speed * GameManager::DeltaTime());
    }
}
