#pragma once
#include "Ennemy.h"
#include "TileMap.h"
#include <vector>

class Boss : public Ennemy
{
private:
	TileMap* m_tileMap = nullptr;
	std::vector<Node<Tile>*> m_currentPath;
	Vector2f32 m_currentNodeIndex = { 0.f, 0.f };
	float m_repathTimer = 0.f;
public:
	std::unordered_map<String, float> m_cooldown;
	std::unordered_map<String, float> m_lastUse;

	float m_time = 0.f;
	//---------Shield----------
	bool m_isShielded = false;
	bool m_hasAlreadyShielded = false;
	//---------Slam-----------
	GameObject* m_slamWave = nullptr;
	std::vector<GameObject*> m_slamWaveSegments;
	bool m_isSlamming = false;
	bool m_hasHitPlayer = false;
	float m_slamRadius = 0.f;
	float m_slamSpeed = 0.1f;
	float m_slamMaxRadius = 20.f;
	float m_slamTimer = 0.f;
	float m_slamDuration = 3.f;
	//---------Laser-----------
	std::vector<GameObject*> m_laserBeamSegments;
	bool m_isLasering = false;
	float m_laserTimer = 0.f;
	float m_laserDamageTimer = 0.f;
	float m_laserDuration = 3.0f;
	float m_laserCurrentYaw = 0.f; 
	float m_laserCurrentPitch = 0.f;
	//------------------------
	// 
	//-------------Functions--------------
	Boss(GameObject* obj, TileMap* tileMap, float spd = 150);
	void Die() override;
	void Attack()override;
	void AddScript();
	float m_ShootCooldown = 0.75f;
	void GeneratePathToPlayer(GameObject* player);
	TileMap* GetTileMap() const { return m_tileMap; }
	std::vector<Node<Tile>*>& GetPath() { return m_currentPath; }
	void SetCurrentTargetNodePosition();
	void FollowPath();
	bool IsReady(const String& atk);
	void Use(const String& atk);
};

