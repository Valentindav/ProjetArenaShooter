#include "Robot.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "StateRobot.h"

using namespace gce;

Robot::Robot(GameObject* obj, float spd) : Ennemy(obj, spd)
{

    GameObject* boxColliderObj = &GameObject::Create(*const_cast<Scene*>(GetGameObject()->GetScene()));
    MeshRenderer* pBoxColliderRenderer = boxColliderObj->AddComponent<MeshRenderer>();
    pBoxColliderRenderer->SetGeometry(RessourcesManager::GetRobot());
    boxColliderObj->transform.SetWorldPosition({ obj->transform.GetWorldPosition().x,obj->transform.GetWorldPosition().y-0.5f,obj->transform.GetWorldPosition().z });

    obj->transform.SetLocalScale({ 1.f, 2.f, 1.f });
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;

    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Robot");
	obj->AddChild(*boxColliderObj);
    obj->transform.SetWorldRotation({ 90.0f, 0.0f, 0.0f });

    obj->transform.SetLocalScale({ 0.7f, 0.7f, 0.7f });


    m_life = 10;
    StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
    String idle = "Idle";
	String Chase = "Chase";
    String attack = "Attack";

    { // ATTACK STATE
        sm->AddAction(attack, &OnStartShootRobot, &OnUpdateShootRobot, &OnEndShootRobot);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Attack") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.SquareNorm() < 30.f;
                }
            }
        );
        sm->AddTransition(conds, attack);
    }
    { // IDLE STATE
        sm->AddAction(idle, &OnStartIdleRobot, &OnUpdateIdleRobot, &OnEndIdleRobot);
        Vector<StateMachine::Condition> conds;
        conds.PushBack(
            {
                [](GameObject* me)->bool {
                    Player* p = RessourcesManager::GetPlayer();
                    if (!p) return false;
                    StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                    if (smLocal && smLocal->actualAction == "Idle") return false;
                    Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                    return d.SquareNorm() > 30.0f;
                }
            }
        );
        sm->AddTransition(conds, idle);
    }
}

void Robot::Die()
{

}

void Robot::Attack()
{

}