#include "Deer.h"
#include "Elf.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "DeerState.h"

using namespace gce;

Deer::Deer(GameObject* obj, TileMap* tileMap, float spd) : Ennemy(obj, spd), m_tileMap(tileMap)
{
    GameObject* boxColliderObj = &GameObject::Create(*const_cast<Scene*>(GetGameObject()->GetScene()));
    MeshRenderer* pBoxColliderRenderer = boxColliderObj->AddComponent<MeshRenderer>();
    pBoxColliderRenderer->SetGeometry(RessourcesManager::GetDeer());
    boxColliderObj->transform.SetWorldPosition({ obj->transform.GetWorldPosition().x,obj->transform.GetWorldPosition().y - 0.5f,obj->transform.GetWorldPosition().z });

    obj->transform.SetLocalScale({ 1.f, 2.f, 1.f });
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Deer");
    obj->AddChild(*boxColliderObj);
    obj->transform.SetLocalScale({ 0.9f, 1.4f, 0.9f });
    m_life = 15.f;

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
    String idle = "Idle";
    String Chase = "Chase";
    String attack = "Attack";

    { // ATTACK STATE
        sm->AddAction(attack, &OnStartShootDeer, &OnUpdateShootDeer, &OnEndShootDeer);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Attack") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() <= 2.0f;
                }
            }
        );
        sm->AddTransition(conds, attack);
    }
    { // IDLE STATE
        sm->AddAction(idle, &OnStartIdleDeer, &OnUpdateIdleDeer, &OnEndIdleDeer);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p || !p->GetGameObject()) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Idle") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() > 2.0f;
                }
            }
        );
        sm->AddTransition(conds, idle);
    }
}

void Deer::Die()
{

}

void Deer::Attack()
{

}

void Deer::SetCurrentTargetNodePosition()
{
    Node<Tile>* node = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    if (node)
    {
        m_currentNodeIndex.x = static_cast<float>(node->data->gridX);
        m_currentNodeIndex.y = static_cast<float>(node->data->gridY);
    }
}

void Deer::GeneratePathToPlayer(GameObject* player)
{
    if (!m_tileMap) return;

    Node<Tile>* startNode = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    Node<Tile>* targetNode = m_tileMap->GetNodeFromWorldPosition(player->transform.GetWorldPosition());

    if (startNode && targetNode && startNode != targetNode)
    {
        m_currentPath = m_tileMap->GeneratePath(startNode, targetNode);
    }
}

void Deer::FollowPath()
{
    if (m_currentPath.size() < 2) return;

    Vector3f32 nextPos = {
        m_currentPath[1]->data->worldPosition.x,
        m_gameObject->transform.GetWorldPosition().y,
        m_currentPath[1]->data->worldPosition.y
    };

    Vector3f32 currentPos = m_gameObject->transform.GetWorldPosition();
    Vector3f32 direction = nextPos - currentPos;

    if (direction.SquareNorm() > 0.01f)
    {
        direction.SelfNormalize();
        float speed = 5.0f;
        m_gameObject->transform.SetWorldPosition(currentPos + direction * speed * GameManager::DeltaTime());
    }
}
