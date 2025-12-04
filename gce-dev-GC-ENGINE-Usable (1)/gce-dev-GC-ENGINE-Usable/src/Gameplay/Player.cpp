#include "Player.h"
#include "Bullet.h"
#include "RessourcesManager.h"
using namespace gce;

DECLARE_SCRIPT(Move, ScriptFlag::Update)
private:
	 Geometry* bulletGeo = GeometryFactory::LoadGeometry("res/Exemple/SUZANNE.obj");
	 Texture* bulletTex = new Texture("res/Exemple/TexturesTest.jpg");
	Bullet* lastBullet = nullptr;

public:
void Update()
{
    gce::LockMouseCursor();
    GameObject* obj = m_pOwner;
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
        obj->GetComponent<PhysicComponent>()->SetGravityScale(-2.0);
    }
    if (GetKey(Keyboard::LCTRL)) {
        obj->GetComponent<PhysicComponent>()->SetGravityScale(2.0);
    }
    if (GetKey(Keyboard::W)) {
        obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0);
    }
if (GetButtonDown(Mouse::LEFT)) {
		GameObject* obj = m_pOwner;
		Scene* scene = const_cast<Scene*>(obj->GetScene());
		GameObject& BulletObject = GameObject::Create(*scene);
		BulletObject.transform.SetWorldPosition(obj->transform.GetWorldPosition());
		BulletObject.transform.SetWorldRotation(obj->transform.GetWorldRotation());
		MeshRenderer* pWeaponRenderer = BulletObject.AddComponent<MeshRenderer>();
		pWeaponRenderer->SetGeometry(bulletGeo);
		Texture* pWeaponTexture = bulletTex;
		pWeaponRenderer->SetAlbedoTexture(pWeaponTexture);
		BulletObject.transform.LocalScale({ 0.25,0.25,0.25 });
		BulletObject.AddComponent<BoxCollider>()->SetActive(true);
		BulletObject.AddComponent<PhysicComponent>();
		BulletObject.GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
		Bullet* bullet = new Bullet(&BulletObject);
		bullet->AddShoot();
        bullet->SetOwner(obj);
        RessourcesManager::AddEntities(bullet);
		lastBullet = bullet;
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

    const float sensitivity = 0.0005f;
    const float pitchMin = -1.4f;
    const float pitchMax = 1.4f;

    yaw += deltaPixels.x * sensitivity;
    pitch += deltaPixels.y * sensitivity;
    pitch = gce::Clamp(pitch, pitchMin, pitchMax);

    Quaternion quaternion = Quaternion::RotationEuler(pitch, yaw, 0.0f);
    obj->transform.SetLocalRotation(quaternion);

    SetMousePosition(center);
}

END_SCRIPT

Player::Player(GameObject* obj, float spd) : Entity(obj, spd) {
    MeshRenderer* pPlayerRenderer = obj->AddComponent<MeshRenderer>();
    pPlayerRenderer->SetGeometry(SHAPES.CUBE);
    Texture* pPlayerTexture = new Texture("res/Exemple/TexturesTest.jpg");
    pPlayerRenderer->SetAlbedoTexture(pPlayerTexture);
    obj->AddComponent<BoxCollider>()->SetActive(true);
    obj->AddComponent<PhysicComponent>();
    obj->GetComponent<PhysicComponent>()->SetGravityScale(0.0f);
    AddMove();
}

void Player::AddMove()
{
    GameObject* obj = GetGameObject();
    obj->AddScript<Move>();
}