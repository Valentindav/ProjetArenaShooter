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
	float m_reloadCD = 1.0f;

	float m_ammo = 15;
	float m_energy = 100.0f;
	float m_meleeCD = 1.0f;
	float m_baseSpeed;
	bool m_realoading = false;

	float m_damage = 1;

	Player(GameObject* obj, float spd = 5);
	void UpdateWeapon();
	void AddMove();
};