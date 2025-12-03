#include "Player.h"

using namespace gce;

DECLARE_SCRIPT(Move, ScriptFlag::Update)

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
