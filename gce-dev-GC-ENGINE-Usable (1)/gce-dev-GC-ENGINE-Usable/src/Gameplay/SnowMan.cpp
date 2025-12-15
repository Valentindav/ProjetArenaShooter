#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "SnowManState.h"

using namespace gce;

SnowMan::SnowMan(GameObject* obj, TileMap* tileMap, float spd) : Ennemy(obj, spd), m_tileMap(tileMap)
{
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("SnowMan");

    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
    String idle = "Idle";
    String Chase = "Chase";
    String attack = "Attack";

    { // ATTACK STATE
        sm->AddAction(attack, &OnStartShootSnowman, &OnUpdateShootSnowman, &OnEndShootSnowman);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Attack") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() < 12.0f;
                }
            }
        );
        sm->AddTransition(conds, attack);
    }
    { // IDLE STATE
        sm->AddAction(idle, &OnStartIdleSnowman, &OnUpdateIdleSnowman, &OnEndIdleSnowman);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p || !p->GetGameObject()) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Idle") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.Norm() > 12.0f;
                }
            }
        );
        sm->AddTransition(conds, idle);

    }
}

void SnowMan::Die()
{

}

void SnowMan::Attack()
{

}

void SnowMan::SetCurrentTargetNodePosition()
{
    Node<Tile>* node = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    if (node)
    {
        m_currentNodeIndex.x = static_cast<float>(node->data->gridX);
        m_currentNodeIndex.y = static_cast<float>(node->data->gridY);
    }
}

void SnowMan::GeneratePathToPlayer(GameObject* player) 
{
    if (!m_tileMap) return;

    Node<Tile>* startNode = m_tileMap->GetNodeFromWorldPosition(m_gameObject->transform.GetWorldPosition());
    Node<Tile>* targetNode = m_tileMap->GetNodeFromWorldPosition(player->transform.GetWorldPosition());

    if (startNode && targetNode && startNode != targetNode)
    {
        m_currentPath = m_tileMap->GeneratePath(startNode, targetNode);
    }
}

void SnowMan::FollowPath()
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
