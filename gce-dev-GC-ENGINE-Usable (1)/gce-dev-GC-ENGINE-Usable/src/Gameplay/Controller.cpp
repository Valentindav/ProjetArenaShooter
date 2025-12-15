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
                obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (player->GetSpeed()*5) * GameManager::DeltaTime());
				player->m_energy -= 20 * GameManager::DeltaTime();
            }
            else {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * player->GetSpeed() * GameManager::DeltaTime());
            }
        }
        if (GetKey(Keyboard::S))
        {
            if (GetKey(Keyboard::LSHIFT) && player->m_energy > 0.0f) {
                obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (-(player->GetSpeed() * 5)) * GameManager::DeltaTime());
                player->m_energy -= 20 * GameManager::DeltaTime();
            }
            else {
                obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (-player->GetSpeed()) * GameManager::DeltaTime());
            }
        }
        if (GetKey(Keyboard::Q) || GetKey(Keyboard::A))
        {
            if (GetKey(Keyboard::LSHIFT) && player->m_energy > 0.0f) {
                obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (-(player->GetSpeed() * 5)) * GameManager::DeltaTime());
                player->m_energy -= 20 * GameManager::DeltaTime();
            }
            else {
                obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (-player->GetSpeed()) * GameManager::DeltaTime());
            }
        }
        if (GetKey(Keyboard::D))
        {
            if (GetKey(Keyboard::LSHIFT) && player->m_energy > 0.0f) {
                obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (player->GetSpeed() * 5) * GameManager::DeltaTime());
                player->m_energy -= 20 * GameManager::DeltaTime();
            }
            else {
                obj->transform.WorldTranslate(obj->transform.GetLocalRight() * player->GetSpeed() * GameManager::DeltaTime());
            }
        }
        if (GetKey(Keyboard::P))
        {
            exit(0);
        }
        if (GetKeyDown(Keyboard::SPACE))
        {
            Move* moveScript = obj->GetScript<Move>();
            PhysicComponent* phys = obj->GetComponent<PhysicComponent>();
            if (moveScript && phys && moveScript->onGround)
            {
                Vector3f32 currentVel = phys->GetVelocity();
                phys->SetVelocity({ currentVel.x, moveScript->jumpForce, currentVel.z });
                moveScript->onGround = false;
            }
        }
        if (GetKey(Keyboard::R))
        {
            player->m_realoading = true;
            if (player->ReloadCD <= 0.0f) {
                player->ammo = 15;
                player->m_realoading = false;
            }
        }
        if (GetKey(Keyboard::LCTRL))
        {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * -(player->GetSpeed()) * GameManager::DeltaTime());
        }
        if (GetKeyDown(Keyboard::_1)) {
            player->m_speed = player->m_baseSpeed;
            if (player->m_weaponLevel == 1) {
                player->m_currentState = Player::GIFT_WEAPON;
				player->m_damage = 1.0f;
			}
            else if (player->m_weaponLevel == 2) {
                player->m_currentState = Player::NERF_WEAPON;
                player->m_damage = 2.0f;
			}
            else if (player->m_weaponLevel == 3) {
                player->m_currentState = Player::THOMPSON_WEAPON;
                player->m_damage = 3.0f;
            }
            player->UpdateWeapon();
        }
        if (GetKeyDown(Keyboard::_2)) {
            player->m_speed = player->m_baseSpeed + 2;
            if (player->m_weaponLevel == 1) {
                player->m_currentState = Player::CANDY_CANE;
				player->m_damage = 2.0f;
            }
            else if (player->m_weaponLevel == 2) {
                player->m_currentState = Player::BROKEN_CANDY_CANE;
                player->m_damage = 3.0f;
            }
            else if (player->m_weaponLevel == 3) {
                player->m_currentState = Player::TESSON;
                player->m_damage = 4.0f;
            }
            player->UpdateWeapon();
        }
        if (GetKeyDown(Keyboard::_3)) {
            player->m_speed = player->m_baseSpeed - 1;
			player->m_currentState = Player::BAZZOKA_WEAPON;
			player->m_damage = 5.0f;
            player->UpdateWeapon();
        }
        if (GetKeyDown(Keyboard::_4)) {
            if (player->m_weaponLevel < 3) {
                player->m_weaponLevel++;
                std::cout << "Weapon level increased to: " << player->m_weaponLevel << std::endl;
                player->UpdateWeapon();
            }
        }
        if (GetKeyDown(Keyboard::_5)) {
            if (player->m_weaponLevel > 1) {
                player->m_weaponLevel--;
                std::cout << "Weapon level increased to: " << player->m_weaponLevel << std::endl;
                player->UpdateWeapon();
            }
        }
        if (GetButtonDown(Mouse::LEFT))
        {
				if (player->m_currentState == Player::GIFT_WEAPON || player->m_currentState == Player::NERF_WEAPON || player->m_currentState == Player::THOMPSON_WEAPON)
                {
                    if (player->ammo > 0 && !player->m_realoading) {
                        Scene* scene = const_cast<Scene*>(obj->GetScene());
                        GameObject& BulletObject = GameObject::Create(*scene);
                        BulletObject.transform.SetWorldPosition(obj->transform.GetWorldPosition());
                        BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());
                        Bullet* bullet = new Bullet(&BulletObject);
                        bullet->SetOwner(obj);
                        obj->GetScript<Move>()->lastBullet = bullet;
                        obj->GetScript<Move>()->m_shootTimer = SHOOT_TIMER_WAIT;
						player->ammo -= 1;
                        player->ReloadCD = 1.0f;
                    } else {
                        player->m_realoading = true;
                        if (player->ReloadCD <= 0.0f) {
							player->ammo = 15;
                            player->m_realoading = false;
                        }
                    }
                }
                else if (player->m_currentState == Player::CANDY_CANE || player->m_currentState == Player::BROKEN_CANDY_CANE || player->m_currentState == Player::TESSON) {
                    if (player->meleeCD < 0.0f) {
                        for (auto entity : RessourcesManager::getEntities()) {
                            if (entity != nullptr && entity->GetGameObject() != nullptr && entity != entityPlayer && player != nullptr) {
                                Vector3f32 d = player->GetGameObject()->transform.GetWorldPosition() - entity->GetGameObject()->transform.GetWorldPosition();
                                if (d.Norm() < 5.0f) {
                                    entity->TakeDamage(player->m_damage);
									player->meleeCD = 1.0f;
                                }
                            }
                        }
                    }
                }
                else if (player->m_currentState == Player::BAZZOKA_WEAPON) {
                    /*RAYCAST CODE + following bullet*/
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
        }
		player->ReloadCD -= GameManager::DeltaTime();
		player->meleeCD -= GameManager::DeltaTime();
    }
