#pragma once
#include "Entity.h"
#include "CameraFeedback.h"
#define SHOOT_TIMER_WAIT 0.0f

class Player :public Entity
{
public:
	enum state {
		GIFT_WEAPON,
		NERF_WEAPON,
		THOMPSON_WEAPON,
		BAZZOKA_WEAPON,
		CANDY_CANE,
		BROKEN_CANDY_CANE,
		TESSON,
	};
	state m_currentState = GIFT_WEAPON;

	CameraFeedback* m_cameraFeedback = nullptr;
	GameObject* m_cameraObject = nullptr;

	int m_weaponLevel = 1;
	float m_reloadCD = 1.0f;

	void SetCamera(GameObject* camera);
	CameraFeedback* GetCameraFeedback() { return m_cameraFeedback; }

	float m_ammo = 15;
	float m_energy = 11.0f;
	float m_meleeCD = 1.0f;
	float m_baseSpeed;
	bool m_realoading = false;

	float m_damage = 1;

	int m_bazooShoot = 1;

	int m_jumpCount = 0;

	Player(GameObject* obj, float spd = 7);
	void UpdateWeapon();
	void AddMove();
	void TakeDamage(int damage) override;

	GameObject* m_cachedWeapon = nullptr;
	bool m_isWeaponAnimating = false;
	float m_weaponAnimTimer = 0.0f;
	int m_weaponAnimPhase = 0;
	Vector3f32 m_weaponOriginalPos;

	void UpdateWeaponAnimation(float deltaTime);
	void TriggerShootAnimation();
	void TriggerReloadAnimation();

	void Heal(float amount) { m_life += amount; if (m_life > 13.f) m_life = 13.f; }
};