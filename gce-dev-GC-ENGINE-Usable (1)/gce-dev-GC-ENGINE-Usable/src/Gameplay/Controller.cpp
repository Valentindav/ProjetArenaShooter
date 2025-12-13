#include "Controller.h"
#include "Bullet.h"
#include "Player.h"
#include "MoveScript.h"
#include "RessourcesManager.h"
using namespace gce;
    void Controller::HandleInput(gce::GameObject* obj)
    {
        Entity* entityPlayer = RessourcesManager::GetEntityFromGameObject(obj);
		Player* player = dynamic_cast<Player*>(entityPlayer);
        if (GetKey(Keyboard::Z) || GetKey(Keyboard::W))
        {
            if (GetKey(Keyboard::LSHIFT) && player->m_energy>0.0f) {
                obj->transform.WorldTranslate(obj->transform.GetLocalForward() * 10 * GameManager::DeltaTime());
				player->m_energy -= 20 * GameManager::DeltaTime();
				std::cout << "Energy: " << player->m_energy << std::endl;
            }
            else {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * 5 * GameManager::DeltaTime());
            }
        }
        if (GetKey(Keyboard::S))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (-5) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::Q) || GetKey(Keyboard::A))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (-5) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::D))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * 5 * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::ESC))
        {
            exit(0);
        }
        if (GetKey(Keyboard::SPACE))
        {
            // Ne plus translater directement ici : la physique doit gérer la position verticale.
            Move* moveScript = obj->GetScript<Move>();
            PhysicComponent* phys = obj->GetComponent<PhysicComponent>();
            if (moveScript && phys && moveScript->onGround)
            {
                // Conserver la vitesse horizontale, appliquer l'impulsion verticale
                Vector3f32 currentVel = phys->GetVelocity();
                phys->SetVelocity({ currentVel.x, moveScript->jumpForce, currentVel.z });
                moveScript->onGround = false;
            }
        }
        if (GetKey(Keyboard::LCTRL))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * -5 * GameManager::DeltaTime());
        }
        if (GetButtonDown(Mouse::LEFT))
        {
            if (obj->GetScript<Move>()->m_shootTimer > 0.0f)
            {
                return;
            }

            for (auto weapon : obj->GetChildren())
            {
                if (weapon->GetName() == "Weapon_1")
                {
                    Scene* scene = const_cast<Scene*>(obj->GetScene());
                    GameObject& BulletObject = GameObject::Create(*scene);
                    BulletObject.transform.SetWorldPosition(obj->transform.GetWorldPosition());
                    BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());
                    Bullet* bullet = new Bullet(&BulletObject);
                    bullet->SetOwner(obj);
                    obj->GetScript<Move>()->lastBullet = bullet;
                    obj->GetScript<Move>()->m_shootTimer = SHOOT_TIMER_WAIT;
                }
            }
        }
        if (GetButtonDown(Mouse::RIGHT))
        {
            if (obj->GetScript<Move>()->lastBullet == nullptr)
            {
                return;
            }

            MeshRenderer* pWeaponRenderer = obj->GetScript<Move>()->lastBullet->GetGameObject()->GetComponent<MeshRenderer>();
            pWeaponRenderer->SetGeometry(SHAPES.CUBE);
            obj->GetScript<Move>()->lastBullet->DeleteShoot();
        }
        if (!GetKey(Keyboard::LSHIFT) && player->m_energy < 100.0f) {
           
            player->m_energy += 10 * GameManager::DeltaTime();
            std::cout << "Energy: " << player->m_energy << std::endl;
        }
    }
