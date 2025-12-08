#include "Player.h"
#include "Bullet.h"
#include "RessourcesManager.h"
#define SHOOT_TIMER_WAIT 0.0f
using namespace gce;

DECLARE_SCRIPT(Move, ScriptFlag::Update | ScriptFlag::CollisionStay)
private:
	 Geometry* bulletGeo = GeometryFactory::LoadGeometry("res/Exemple/bottle.obj");
	 /*Texture* bulletTex = new Texture("res/Exemple/TexturesTest.jpg");*/
	 Bullet* lastBullet = nullptr;
	 float m_shootTimer = SHOOT_TIMER_WAIT;
	 bool onGround = false;
     float sensitivity = 0.0005f;
	 float jumpForce = 50.0f;

public:
    void Update()
    {
        gce::LockMouseCursor();
        GameObject* obj = m_pOwner;
        float32 gravity = obj->GetComponent<PhysicComponent>()->GetGravityScale();        
        obj->GetComponent<PhysicComponent>()->SetBounciness(-1.0f);
        if (GetKey(Keyboard::Z)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * 2 * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::S)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalForward() * (-2) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::Q)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * (-2) * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::D)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalRight() * 2 * GameManager::DeltaTime());
        }
        if (GetKey(Keyboard::SPACE)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * 2 * GameManager::DeltaTime());
          /*  if (onGround)
            {                
                obj->GetComponent<PhysicComponent>()->SetVelocity({ 0.0f,jumpForce,0.0f });
                onGround = false;
            }*/
        }
        if (GetKey(Keyboard::LCTRL)) {
            obj->transform.WorldTranslate(obj->transform.GetLocalUp() * -2 * GameManager::DeltaTime());
        }
        if (GetButtonDown(Mouse::LEFT)) {
            if (m_shootTimer > 0.0f)  return; 
            for (auto weapon : obj->GetChildren()) {
                if (weapon->GetName() == "Weapon_1") {
                    GameObject* obj = m_pOwner;
                    Scene* scene = const_cast<Scene*>(obj->GetScene());
                    GameObject& BulletObject = GameObject::Create(*scene);
                    BulletObject.transform.SetWorldPosition(obj->transform.GetWorldPosition());
                    BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());
                    MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
                    pWeaponRenderer->SetGeometry(bulletGeo);
                    /*Texture* pWeaponTexture = bulletTex;
                    pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);*/
                    BulletObject.transform.LocalScale({ 0.05,0.05,0.05 });
                    BulletObject.AddComponent<BoxCollider>()->SetActive(false);
                    BulletObject.AddComponent<PhysicComponent>();
                    BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
                    Bullet* bullet = new Bullet(&BulletObject);
                    bullet->AddShoot();
                    lastBullet = bullet;
                    m_shootTimer = SHOOT_TIMER_WAIT;
                }
            }
        }
        if (GetButtonDown(Mouse::RIGHT)) {
            if (lastBullet == nullptr) return;
            MeshRenderer* pWeaponRenderer = lastBullet->GetGameObject()->GetComponent<MeshRenderer>();
            pWeaponRenderer->SetGeometry(SHAPES.CUBE);
            lastBullet->DeleteShoot();
        }
        gce::WindowParam windowParam = GameManager::GetWindowParam();
        gce::Vector2i32 const center = { windowParam.width / 2, windowParam.height / 2 };
        gce::Vector2i32 const currentPos = GetMousePosition();
        gce::Vector2f32 const deltaPixels = currentPos - center;

        static float yaw = 0.0f;
        static float pitch = 0.0f;
        
        const float pitchMin = -1.4f;
        const float pitchMax = 1.4f;

        yaw += deltaPixels.x * sensitivity;
        pitch += deltaPixels.y * sensitivity;
        pitch = gce::Clamp(pitch, pitchMin, pitchMax);

        Quaternion quaternion = Quaternion::RotationEuler(pitch, yaw, 0.0f);
        obj->transform.SetLocalRotation(quaternion);

        SetMousePosition(center);
        m_shootTimer -= GameManager::DeltaTime();
    }

    void CollisionStay(GameObject* other) {
        if (m_pOwner && m_pOwner->IsActive())
        {
            if (other->GetName() == "Floor")
            {
                onGround = true;
            }
        }
    }

END_SCRIPT

Player::Player(GameObject* obj, float spd) : Entity(obj, spd) {
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
	obj->GetComponent<BoxCollider>()->isTrigger = false;
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(9.81f);
    obj->SetName("Player");
    AddMove();
    AddLifeScript();
}

void Player::AddMove()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<Move>();
}