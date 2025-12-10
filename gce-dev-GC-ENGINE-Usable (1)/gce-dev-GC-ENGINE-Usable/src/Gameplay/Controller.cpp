#include "Controller.h"
#include "Bullet.h"
#include "Player.h"
#include "MoveScript.h"

using namespace gce;
    void Controller::HandleInput(gce::GameObject* obj)
    {
        if (GetKey(Keyboard::Z) || GetKey(Keyboard::W))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * 2 * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::S))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (-2) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::Q) || GetKey(Keyboard::A))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (-2) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::D))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * 2 * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::ESC))
        {
            exit(0);
        }
        if (GetKey(Keyboard::SPACE))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * 2 * GameManager::DeltaTime());
            if (obj->GetScript<Move>()->onGround)
            {
                obj->GetComponent<PhysicComponent>()->SetVelocity({ 0.0f,obj->GetScript<Move>()->jumpForce,0.0f });
                obj->GetScript<Move>()->onGround = false;
            }
        }
        if (GetKey(Keyboard::LCTRL))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * -2 * GameManager::DeltaTime());
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
    }
