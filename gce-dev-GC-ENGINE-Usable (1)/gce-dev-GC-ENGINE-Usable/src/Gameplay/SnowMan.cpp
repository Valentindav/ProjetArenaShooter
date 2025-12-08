#include "SnowMan.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Engine/StateMachine.h"
#include "Player.h"
#include "Engine.h"
#include "Entity.h"
#include "SnowManState.cpp"

using namespace gce;

    SnowMan::SnowMan(GameObject* obj, float spd) : Ennemy(obj, spd)
    {
        MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
        pPlayerRenderer->SetGeometry(SHAPES.CUBE);
        Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
        pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
        obj->AddComponent<BoxCollider>()->SetActive(true);
        obj->AddComponent<PhysicComponent>();
        obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
        obj->SetName("SnowMan");

        StateMachine* sm = GameManager::GetStatesSystem().CreateStateMachine(obj);
        String idle = "Idle";
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
                        if (!p) return false;
                        StateMachine* smLocal = GameManager::GetStatesSystem().CreateStateMachine(me);
                        if (smLocal && smLocal->actualAction == "Idle") return false;
                        Vector3f32 d = p->GetGameObject()->transform.GetWorldPosition() - me->transform.GetWorldPosition();
                        return d.Norm() > 12.0f;
                    }
                }
            );
            sm->AddTransition(conds, idle);
        }
        AddLifeScript();
    }
        
void SnowMan::Die()
{
    
}

void SnowMan::Attack()
{
   
}
