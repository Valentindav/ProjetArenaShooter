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
	std::unordered_map<String, float> m_cooldown;
	std::unordered_map<String, float> m_lastUse;
public:
	//---------Shield----------
	bool m_isShielded = false;
	bool m_hasAlreadyShielded = false;


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

