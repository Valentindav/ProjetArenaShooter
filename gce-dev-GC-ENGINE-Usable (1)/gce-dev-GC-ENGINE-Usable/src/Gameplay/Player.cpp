#include "Player.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#include "Controller.h"
#include "MoveScript.h"

using namespace gce;

Player::Player(GameObject* obj, float spd) : Entity(obj, spd)
{
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    m_life = 14.f;  // CORRIGÉ : était 14.f
    obj->transform.SetLocalScale({ 2.0f, 2.0f, 2.0f });
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Player");
    m_weaponLevel = 1;
    m_baseSpeed = spd;
    m_bazooShoot = 1;
    AddMove();

    for (auto child : obj->GetChildren()) {
        if (child->GetName() == "Weapon_1") {
            m_cachedWeapon = child;
            break;
        }
    }

    m_weaponOriginalPos = Vector3f32(0.0f, 0.0f, 0.0f);
}

void Player::SetCamera(GameObject* camera)
{
    m_cameraObject = camera;
    if (m_cameraFeedback)
        delete m_cameraFeedback;
    m_cameraFeedback = new CameraFeedback(camera);
}

void Player::UpdateWeapon()
{
    GameObject* obj = nullptr;
    for (auto child : GetGameObject()->GetChildren()) {
        if (child->GetName() == "Weapon_1") {
            obj = child;
        }
    }
    if (obj == nullptr) return;
    MeshRenderer* pChildRenderer = obj->GetComponent<MeshRenderer>();
    switch (m_currentState) {
    case GIFT_WEAPON:
        pChildRenderer->SetGeometry(RessourcesManager::GetGift());
		break;
    case NERF_WEAPON:
        pChildRenderer->SetGeometry(RessourcesManager::GetNerf());
        break;
    case THOMPSON_WEAPON:
        pChildRenderer->SetGeometry(RessourcesManager::GetThomson());
        break;
    case CANDY_CANE:
        pChildRenderer->SetGeometry(RessourcesManager::GetsurgarCane());
        break;
    case BROKEN_CANDY_CANE:
        pChildRenderer->SetGeometry(RessourcesManager::GetBrokenSugarCane());
        break;
    case TESSON:
        pChildRenderer->SetGeometry(RessourcesManager::GetBottle());
        break;
    case BAZZOKA_WEAPON:
        pChildRenderer->SetGeometry(RessourcesManager::GetElf());
    }
}

void Player::AddMove()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<Move>();
}

void Player::TakeDamage(int damage)
{
    if (this->m_life <= 0)
    {
        RessourcesManager::SetPlayer(nullptr);
        this->GetGameObject()->SetActive(false);
        this->GetGameObject()->Destroy();

        RessourcesManager::RemoveEntities(this);
        delete this;
    }
    else
    {
        this->m_life = this->m_life - damage;
        if (m_cameraFeedback)
            m_cameraFeedback->TriggerDamageShake();
    }
}

void Player::TriggerShootAnimation()
{
    m_isWeaponAnimating = true;
    m_weaponAnimTimer = 0.0f;
    m_weaponAnimPhase = 1;
}

void Player::TriggerReloadAnimation()
{
    m_isWeaponAnimating = true;
    m_weaponAnimTimer = 0.0f;
    m_weaponAnimPhase = 2;
}

void Player::UpdateWeaponAnimation(float deltaTime)
{
    if (!m_isWeaponAnimating || !m_cachedWeapon) return;

    GameObject* weaponObj = m_cachedWeapon;

    if (!weaponObj)
    {
        return;
    }

    if (m_weaponAnimTimer == 0.0f && (m_weaponOriginalPos.x == 0.0f && m_weaponOriginalPos.y == 0.0f && m_weaponOriginalPos.z == 0.0f))
    {
        m_weaponOriginalPos = weaponObj->transform.GetLocalPosition();
    }

    m_weaponAnimTimer += deltaTime;

    if (m_weaponAnimPhase == 1)
    {
        float shootDuration = 0.3f;

        if (m_weaponAnimTimer >= shootDuration)
        {
            m_isWeaponAnimating = false;
            weaponObj->transform.SetLocalPosition(m_weaponOriginalPos);
            return;
        }

        float progress = m_weaponAnimTimer / shootDuration;

        float offset;
        if (progress < 0.4f)
        {
            offset = -(progress / 0.4f) * 0.3f;
        }
        else
        {
            float returnProgress = (progress - 0.4f) / 0.6f;
            offset = -0.3f * (1.0f - returnProgress);
        }

        Vector3f32 newPos = m_weaponOriginalPos;
        newPos.z += offset;

        weaponObj->transform.SetLocalPosition(newPos);
    }
    else if (m_weaponAnimPhase == 2)
    {
        float reloadDuration = 1.5f;

        if (m_weaponAnimTimer >= reloadDuration)
        {
            m_isWeaponAnimating = false;
            weaponObj->transform.SetLocalPosition(m_weaponOriginalPos);
            return;
        }

        float progress = m_weaponAnimTimer / reloadDuration;

        Vector3f32 newPos = m_weaponOriginalPos;

        if (progress < 0.3f)
        {
            float dropProgress = progress / 0.3f;
            newPos.y -= dropProgress * 0.8f;
        }
        else if (progress < 0.7f)
        {
            float sideProgress = (progress - 0.3f) / 0.4f;
            newPos.y -= 0.8f;
            newPos.y += sideProgress * 1.2f;
        }

        weaponObj->transform.SetLocalPosition(newPos);
    }
}