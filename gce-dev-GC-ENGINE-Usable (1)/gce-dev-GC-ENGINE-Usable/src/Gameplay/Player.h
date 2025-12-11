#pragma once
#include "Entity.h"
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

	int m_weaponLevel = 1;
	float ReloadCD = 1.0f;

	float ammo = 15;
	float m_energy = 100.0f;
	float meleeCD = 1.0f;
	float m_baseSpeed;
	bool m_realoading = false;

	float m_weaponDamage = 1;
	float m_melleeDamage = 1;

	Player(GameObject* obj, float spd = 150);
	void UpdateWeapon();
	void AddMove();
};