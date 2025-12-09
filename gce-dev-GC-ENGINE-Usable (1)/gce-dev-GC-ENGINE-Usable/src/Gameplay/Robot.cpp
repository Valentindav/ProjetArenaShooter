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
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Textures/crosshair.png");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
    obj->SetName("robot");

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
                    return d.Norm() < 30.0f;
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
                    return d.Norm() > 30.0f;
                }
            }
        );
        sm->AddTransition(conds, idle);
    }
    AddLifeScript();
}

void Robot::Die()
{

}

void Robot::Attack()
{

}
