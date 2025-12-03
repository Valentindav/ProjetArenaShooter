#include "Player.h"

using namespace gce;

DECLARE_SCRIPT(Move, ScriptFlag::Update)

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
		obj->transform.WorldTranslate(obj->transform.GetLocalUp() * 2 * GameManager::DeltaTime());
	}
	if (GetKey(Keyboard::LCTRL)) {
		obj->transform.WorldTranslate(obj->transform.GetLocalUp() * (-2) * GameManager::DeltaTime());
	}
	gce::WindowParam windowParam = GameManager::GetWindowParam();
	gce::Vector2i32 const center = { windowParam.width / 2, windowParam.height / 2 };
	gce::Vector2i32 const currentPos = GetMousePosition();
	gce::Vector2f32 const deltaPixels = currentPos - center;

	static float yaw = 0.0f;
	static float pitch = 0.0f;
	static float prevYaw = 0.0f;
	static float prevPitch = 0.0f;

	const float sensitivity = 0.0005f;
	const float pitchMin = -1.4f;
	const float pitchMax = 1.4f;

	float yawDelta = deltaPixels.x * sensitivity;
	float pitchDelta = deltaPixels.y * sensitivity;

	yaw += yawDelta;
	pitch += pitchDelta;
	
	pitch = gce::Clamp(pitch, pitchMin, pitchMax);

	float applyPitch = pitch - prevPitch;
	float applyYaw = yaw - prevYaw;

	obj->transform.LocalRotate({ applyPitch, applyYaw, 0.0f });	

	prevPitch = pitch;
	prevYaw = yaw;

	SetMousePosition(center);
}

END_SCRIPT

void Player::MovePlayer() {		
	
}

void Player::AddMove()
{
	GameObject* obj = GetGameObject();
	obj->AddScript<Move>();
}
