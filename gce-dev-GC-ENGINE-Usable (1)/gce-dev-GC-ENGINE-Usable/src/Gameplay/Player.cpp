#include "Player.h"
#include "Bullet.h"
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
	static gce::Vector2i32 lastPosition = GetMousePosition();
	gce::Vector2i32 const currentPosition = GetMousePosition();
	gce::Vector2f32 const delta = gce::Vector2f32(static_cast<float32>(currentPosition.x - lastPosition.x), static_cast<float32>(currentPosition.y - lastPosition.y));
	lastPosition = currentPosition;
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
		obj->transform.WorldTranslate(obj->transform.GetLocalUp() * 2 * GameManager::DeltaTime());
	}
	if (GetKey(Keyboard::LCTRL)) {
		obj->transform.WorldTranslate(obj->transform.GetLocalUp() * (-2) * GameManager::DeltaTime());
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
		lastBullet = bullet;
	}
	if (GetButtonDown(Mouse::RIGHT)) {
		if (lastBullet == nullptr) return;
		MeshRenderer* pWeaponRenderer = lastBullet->GetGameObject()->GetComponent<MeshRenderer>();
		pWeaponRenderer->SetGeometry(SHAPES.CUBE);
		lastBullet->DeleteShoot();
	}
	obj->transform.LocalRotate({ delta.y * .005f,delta.x * 0.005f,.0f });		
}

END_SCRIPT

void Player::MovePlayer() {		
	
}

void Player::AddMove()
{
	GameObject* obj = GetGameObject();
	obj->AddScript<Move>();
}
